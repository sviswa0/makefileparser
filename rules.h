typedef struct List {
    char *name;
    struct List *next;
} Dep, Action;

typedef struct ActionList {
    Action *head;
} ActionList;

typedef struct Rule {
    char *target;
    Dep *deproot;
    ActionList *actroot;
    struct Rule *next;
} Rule;

Rule *createRule(char *t, Dep *root);
ActionList* addAction(ActionList *lst, char *act);
Dep * getDependencies(char *d);
Rule *findRuleFromTarget(Rule *root, char *targName);
int fileExists(char *fName);
int isUpdated(char *target, char *dependency);
Dep *reverseDepList(Dep *root);
Rule *reverseLinkedList(Rule *root);
int validateDependency(Rule *root, Dep *dptr);
Rule *findRule(Rule *root, char *depName);
void executeActions(ActionList *ptr);
int traverseRules(Rule *root, Rule *trav);
void printRules(Rule *trav);
void freeAction(struct List *root);
void freeDep(struct List *root);
void freeRules(Rule *root);

char *trim(char *s);

