#include <stdarg.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/queue.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

#include <errno.h>

#include "messages.h"
#include "timing.h"
#include "netorder.h" 
#include "mg_files.h"
#include "bitio_gen.h"
#include "bitio_m_mem.h"
#include "bitio_m.h"
#include "memlib.h"
#include "local_strings.h"  /* [RPAP - Feb 97: Term Frequency] */

#include "filestats.h"
#include "invf.h"
#include "text.h"
#include "mg.h"
#include "lists.h"
#include "backend.h"
#include "stemmer.h"
#include "environment.h"
#include "globals.h"
#include "read_line.h"
#include "mg_errors.h"
#include "commands.h"
#include "text_get.h"
#include "term_lists.h"
#include "query_term_list.h"
#include "words.h"


struct node {
  struct node *left;
  struct node *right;
  int DocNum;
  float Score;
};


void destroy (struct node *root) {
  if(root->left)
    destroy(root->left);
  if(root->right)
    destroy(root->right);
  free(root);
}

void insert (struct node *root, struct node *n) {
  
  if(n->Score < root->Score) {
    if(root->left == NULL)
      root->left = n;
    else
      insert (root->left, n);
  } else {
    if(root->right == NULL)
      root->right = n;
    else
      insert (root->right, n);
  }
}

int g = 0;

void traverse (FILE *fd, struct node *root, int start, int num, int c) {

  if(root->right)
    traverse(fd, root->right, start, num, c);
  if ((g >=start) && (g < start + num)) {
    fprintf(fd, "%d\t%f\n", root->DocNum, root->Score);
  }
  g++;
  if(root->left)
    traverse(fd, root->left, start, num, c);
  
}


int 
doc_count_comp (const void *A, const void *B)
{
  const TermEntry *a = A;
  const TermEntry *b = B;
  return (a->WE.doc_count - b->WE.doc_count);
}


invf_data *
InitInvfFile (FILE * InvfFile, stemmed_dict * sd)
{
  invf_data *id;
  if (!(id = Xmalloc (sizeof (invf_data))))
    {
      mg_errno = MG_NOMEM;
      return NULL;
    }

  //  id->InvfFile = InvfFile;

  fread (&id->ifh, sizeof (id->ifh), 1, InvfFile);
  /* [RPAP - Jan 97: Endian Ordering] */
  
  {
    int i;
    
    NTOHUL(id->ifh.no_of_words);
    NTOHUL(id->ifh.no_of_ptrs);
    NTOHUL(id->ifh.skip_mode);
    for (i = 0; i < 16; i++)
      NTOHUL(id->ifh.params[i]);
    NTOHUL(id->ifh.InvfLevel);
  }
  

  id->N = sd->sdh.num_of_docs;
  id->Nstatic = sd->sdh.static_num_of_docs;

  return (id);
}



static TermList *
ParseRankedQuery (stemmed_dict * sd, char *QueryLine, int Sort, int indexed,
		  QueryTermList **query_term_list)  /* [RPAP - Feb 97: Term Frequency] */
{
  u_char Word[MAXSTEMLEN + 1];
  u_char sWord[MAXSTEMLEN + 1];
  u_char *end, *s_in;
  int default_stem_method = 0;
  TermList *Terms = MakeTermList(0);

  s_in = (u_char *) QueryLine;
  end = s_in + strlen ((char *) s_in) - 1;
  *query_term_list = MakeQueryTermList(0);  /* [RPAP - Feb 97: Term Frequency] */

  if (indexed)
    default_stem_method = 3;
  else
    default_stem_method = sd->sdh.stem_method;

  while (s_in <= end)
    {
      int j;
      long num_entries, word_num;
      unsigned long count, doc_count, invf_ptr, invf_len;
      int weight_to_apply, stem_to_apply;
      int method_using = -1;

      /* 0=optional, 1=mustmatch */
      int require_match = 0; /* [RJM 07/97: Ranked Required Terms] */

      /* Skip over the non word separator taking note of any parameters */
      PARSE_RANKED_NON_STEM_WORD (require_match, s_in, end); /* [RJM 07/97: Ranked Required Terms] */
      if (s_in > end) break;

      /* Get a word and stem it */
      PARSE_STEM_WORD (Word, s_in, end);

      /* Extract any parameters */
      weight_to_apply = 1;
      stem_to_apply = default_stem_method;
      while (s_in <= end)
	{
	  int stem_param, weight_param, param_type;
	  char param[MAXPARAMLEN + 1];

	  param_type = 0;
	  PARSE_OPT_TERM_PARAM (param, param_type, s_in, end);
	  if (!param_type)
	    break;

	  switch (param_type)
	    {
	    case (WEIGHTPARAM):
	      weight_param = atoi (param);
	      if (errno != ERANGE && weight_param > 0)
		weight_to_apply = weight_param;
	      break;

	    case (STEMPARAM):
	      stem_param = atoi (param);
	      if (errno != ERANGE && indexed && stem_param >= 0 && stem_param <= 3)
		method_using = stem_to_apply = stem_param;
	      break;
	    }
	}

      bcopy ((char *) Word, (char *) sWord, *Word + 1);
      stemmer (stem_to_apply, sWord);

      if (!indexed || stem_to_apply == 0)
	{
	  /* Look for the word in the already identified terms */
	  for (j = 0; j < Terms->num; j++)
	    if (compare (Terms->TE[j].Word, Word) == 0)
	      break;

	  /* Increment the weight if the word is in the list */
	  /* Update the require match attribute */
	  if (j < Terms->num)
	    {
	      Terms->TE[j].Count = ((Terms->TE[j].Count + weight_to_apply > INT_MAX) ?
				    INT_MAX : (Terms->TE[j].Count + weight_to_apply));
	      Terms->TE[j].require_match = require_match; /* [RJM 07/97: Ranked Require match] */
	      AddQueryTerm (query_term_list, Word, Terms->TE[j].WE.count, method_using);  /* [RPAP - Feb 97: Term Frequency] */
	    }
	  else
	    {
	      /* Look for it in the stemmed dictionary */
	      if ((word_num = FindWord (sd, sWord, &count, &doc_count,
					&invf_ptr, &invf_len)) != -1)
		{
		  /* Search the list for the word */
		  for (j = 0; j < Terms->num; j++)
		    if (Terms->TE[j].WE.word_num == word_num)
		      break;

		  /* Increment the weight if the word is in the list */
		  if (j < Terms->num)
		    {
		      Terms->TE[j].Count = ((Terms->TE[j].Count + weight_to_apply > INT_MAX) ?
					    INT_MAX : (Terms->TE[j].Count + weight_to_apply));
		      Terms->TE[j].require_match = require_match; /* [RJM 07/97: Ranked Require match] */
		      AddQueryTerm (query_term_list, Word, Terms->TE[j].WE.count, method_using);  /* [RPAP - Feb 97: Term Frequency] */
		    }
		  else
		    {
		      /* Create a new entry in the list for the new word */
		      TermEntry te;

		      te.WE.word_num = word_num;
		      te.WE.count = count;
		      te.WE.doc_count = doc_count;
		      te.WE.max_doc_count = doc_count;
		      te.WE.invf_ptr = invf_ptr;
		      te.WE.invf_len = invf_len;
		      te.Count = weight_to_apply;
		      te.Word = copy_string (Word);
		      if (!te.Word)
			FatalError (1, "Could NOT create memory to add term");
		      te.Stem = NULL;
		      te.require_match = require_match;

		      AddTermEntry (&Terms, &te);

		      /* [RPAP - Feb 97: Term Frequency] */
		      AddQueryTerm (query_term_list, Word, count, method_using);
		    }
		}
	      /* [RPAP - Feb 97: Term Frequency] */
	      else
		AddQueryTerm (query_term_list, Word, 0, method_using);
	    }
	}

    } /* end while */

  if (Sort)
    /* Sort the terms in ascending order by doc_count */
    qsort (Terms->TE, Terms->num, sizeof (TermEntry), doc_count_comp);
  return (Terms);
}

#ifdef poo
static File *
OpenFile (char *base, char *suffix, unsigned long magic, int *ok)
{
  char FileName[512];
  File *F;
  sprintf (FileName, "%s%s", base, suffix);
  if (!(F = Fopen (FileName, "rb", 0)))  /* [RPAP - Feb 97: WIN32 Port] */
    {
      mg_errno = MG_NOFILE;
      MgErrorData (FileName);
      if (ok)
	*ok = 0;
      return (NULL);
    }
  if (magic)
    {
      unsigned long m;
      if (fread ((char *) &m, sizeof (m), 1, F->f) == 0)
	{
	  mg_errno = MG_READERR;
	  MgErrorData (FileName);
	  if (ok)
	    *ok = 0;
	  Fclose (F);
	  return (NULL);
	}
      NTOHUL(m); /* [RPAP - Jan 97: Endian Ordering] */
      if (m != magic)
	{
	  mg_errno = MG_BADMAGIC;
	  MgErrorData (FileName);
	  if (ok)
	    *ok = 0;
	  Fclose (F);
	  return (NULL);
	}
    }
  return (F);
}
#endif

extern int passiveTCP(const char *, int);
extern void GetPostProc (char *);

stemmed_dict *sd;
invf_data    *id;
char         *invf_file;
float        *weights_file;

/* mmap invf_file */

struct node *
CosineDecode (TermList *Terms)
{

  struct _accum {
    float Weight;
    char  Terms;
  } *Accum, *wp;

  int    num_docs;
  int    num_terms;
  register int    j,p;
  WordEntry *we;
  int    blk;
  double Wqt, WordLog;
  unsigned long CurrDocNum;
  struct node *n, *root = NULL;

  num_docs = sd->sdh.num_of_docs;

  Accum = mmap(NULL, num_docs * sizeof(struct _accum), (PROT_READ|PROT_WRITE), MAP_ANON, -1, 0);

  if((int)Accum == -1) {
    perror("mmap");
    exit(1);
  }

  num_terms = Terms->num;

  for (j=0; j<num_terms; j++) {
    wp = Accum - 1;
    CurrDocNum = 0;
    we = &Terms->TE[j].WE;
    blk = BIO_Bblock_Init (id->Nstatic, we->doc_count); /* from CalcBlks */

    DECODE_START(invf_file + we->invf_ptr, we->invf_len);

    WordLog = log ((double) num_docs / (double) we->doc_count);
    Wqt = Terms->TE[j].Count * WordLog;
    
    for (p = we->doc_count; p; p--)
      {
	int Count;		/* The number of times the occurs in a document */
	double Wdt;
	unsigned long diff;

	BBLOCK_DECODE (diff, blk);
	CurrDocNum += diff;
	wp += diff;

	GAMMA_DECODE (Count);

	Wdt = sqrt(Count) * WordLog;
	wp->Weight += Wqt * Wdt;
	wp->Terms++;

	if(wp->Terms == num_terms) {
	  n = malloc(sizeof(struct node));
	  n->right = NULL;
	  n->left = NULL;
	  n->DocNum = CurrDocNum;
	  n->Score = wp->Weight / *(weights_file + CurrDocNum - 1);

	  if(!root)
	    root = n;
	  else
	    insert(root, n);

	}

      }

    DECODE_DONE;
    
  }
  munmap(Accum, num_docs * sizeof(struct _accum));
  return root;    
}

struct _DE {
  LIST_ENTRY(_DE) entries;
  unsigned long DocNum;
  float Score;
};



void 
RankedQuery (char *query, int fd)
{
  TermList *TL;
  QueryTermList *QTL;
  struct node *root;
  char buf[1024];
  FILE *f;
  int *start, *num;

  start = (int *)query;
  query += sizeof(int);
  num = (int *)query;
  query += sizeof(int);

  TL = ParseRankedQuery(sd, query, 1, 0, &QTL);
  root = CosineDecode(TL);

  f = fdopen(fd, "r+");
  g = 0;
  if(root) {
    traverse(f, root, *start, *num, 0);
    destroy(root);
  }
  fprintf(f, "END\n");
  fclose(f);

  FreeTermList(&TL);
  FreeQueryTermList(&QTL);
}


void init (void) {
  int fd1, fd2;
  struct stat sb1, sb2;
  FILE *poo1;
  FILE *poo2;
  int ok;

  if ((fd1 = open("/home/osdigger/mgdata/test2/test2.invf", O_RDONLY)) == -1) {
    perror("unable to open .invf file");
    exit(1);
  }

  if ((fd2 = open("/home/osdigger/mgdata/test2/test2.weight", O_RDONLY)) == -1) {
    perror("unable to open .weight file");
    exit(1);
  }

  fstat(fd1, &sb1);

  if((invf_file = mmap(NULL, sb1.st_size, PROT_READ, MAP_PRIVATE, fd1, 0)) == (char *) -1) {
    perror("unable to mmap .invf file");
    exit(1);
  }  
  
  fstat(fd2, &sb2);

  if((weights_file = mmap(NULL, sb2.st_size, PROT_READ, MAP_PRIVATE, fd2, 0)) == (float *) -1) {
    perror("unable to mmap .weight file");
    exit(1);
  }
  weights_file += sizeof(unsigned long);

  //  poo1 = OpenFile ("/home/osdigger/mgdata/test2/test2", INVF_DICT_BLOCKED_SUFFIX, MAGIC_STEM, &ok);
  poo1 = fopen("/home/osdigger/mgdata/test2/test2.invf.dict.blocked", "r");
  fseek(poo1, sizeof(unsigned long), 0);
  sd = (stemmed_dict *) ReadStemDictBlk(poo1);

  //  poo2 = OpenFile ("/home/osdigger/mgdata/test2/test2", INVF_SUFFIX, MAGIC_INVF, &ok);
  poo2 = fopen("/home/osdigger/mgdata/test2/test2.invf", "r");
  fseek(poo2, sizeof(unsigned long), 0);
  id = InitInvfFile(poo2, sd);

  //  RankedQuery("linux install");


}

