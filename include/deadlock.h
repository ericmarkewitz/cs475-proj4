//TODO - add your deadlock function prototypes

void rag_request(int pid, int lockid);
void rag_alloc(int pid, int lockid);
void rag_dealloc(int pid, int lockid);
void rag_print();
void print_arr(int arr[]);
void deadlock_detect();
void deep_copy_arr(int arr[], int copyArr[]);
int deadlock_detect_Helper(int pid, int visited[], int order[]);
//void clear_rag();
void print_order(int arr[]);

void deadlock_recover();


//TODO - add an "extern" declaration for the RAG
//extern int RAG[NLOCK+NPROC][NLOCK+NPROC];
extern int RAG[NLOCK+NPROC][NLOCK+NPROC];


