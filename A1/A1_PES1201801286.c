#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node
{
    char key[1024];
    struct node *left;
    struct node *right;
};
typedef struct node node_t;

struct tree
{
    node_t *root;
};
typedef struct tree tree_t;

struct pathnode
{
    node_t *ptr;
    struct pathnode *next;
};
typedef struct pathnode pathnode_t;

struct pathtree
{
    pathnode_t *head;
};
typedef struct pathtree pathtree_t;

static pathnode_t *make_pathnode(node_t *ptr_node)
{
    pathnode_t *temp = (pathnode_t *)malloc(sizeof(pathnode_t));
    temp->ptr = ptr_node;
    temp->next = NULL;
    return temp;
}

static node_t *make_node(const char *name)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    strcpy(temp->key, name);
    temp->left = NULL;
    temp->right = NULL;
    return temp;
}

void init_tree(tree_t *ptr_tree)
{
    ptr_tree->root = make_node("root");
}

void init_path(pathtree_t *ptr_path, tree_t *ptr_tree)
{
    ptr_path->head = make_pathnode(ptr_tree->root);
}

void add_child_to_parent(node_t *parent, node_t *child)
{
    if (parent->left == NULL)
    {
        parent->left = child;
    }
    else
    {
        node_t *temp = parent->left;
        while (temp->right != NULL)
        {
            temp = temp->right;
        }
        temp->right = child;
    }
}

void gen_header()
{
    printf("#include <stdio.h>\n");
    printf("#include <stdlib.h>\n\n");
}

void prologue()
{
    printf("\nint main()\n");
    printf("{\n");
}

void epilogue()
{
    printf("\treturn 0;\n");
    printf("}\n");
}

void gen_funcs(node_t *t)
{
    node_t *temp = t->left;
    while (temp != NULL) //for each child
    {
        printf("\nvoid %s()\n", temp->key);
        printf("{\n");
        printf("\tprintf(\"%s\\n\");\n", temp->key);
        if (temp->left == NULL)
        {
            printf("\treturn;\n");
        }
        else
        {
            printf("\tint choice;\n");
            printf("\twhile(scanf(\"%%d\",&choice) != 0 )\n");
            printf("\t{\n");
            printf("\t\tswitch(choice)\n");
            printf("\t\t{\n");
            printf("\t\t\tcase 0:\n");
            printf("\t\t\t\treturn;\n");
            node_t *coc = temp->left;
            int i = 1;
            while (coc != NULL) //for each subchild of child
            {
                printf("\t\t\tcase %d:\n", i);
                printf("\t\t\t\t%s();\n", coc->key);
                printf("\t\t\t\tbreak;\n");
                ++i;
                coc = coc->right;
            }
            printf("\t\t\tdefault:\n");
            printf("\t\t\t\tbreak;\n");
            printf("\t\t}\n");
            printf("\t}\n");
        }
        printf("}\n");

        temp = temp->right;
    }
}

//Call gen_funcs on this node ==> All childrens' funcs are generated
//call gen_func_r on each of its child so that for each subchild, funcs are generated.
void gen_func_r(node_t *cn)
{
    gen_funcs(cn);
    node_t *temp = cn->left;
    while (temp != NULL)
    {
        gen_func_r(temp);
        temp = temp->right;
    }
}

void gen_root(node_t *temp)
{
    printf("\nvoid %s()\n", temp->key);
    printf("{\n");
    printf("\tint choice;\n");
    printf("\twhile(scanf(\"%%d\",&choice) != 0 )\n");
    printf("\t{\n");
    printf("\t\tswitch(choice)\n");
    printf("\t\t{\n");
    printf("\t\t\tcase 0:\n");
    printf("\t\t\t\texit(0);\n");
    node_t *coc = temp->left;
    int i = 1;
    while (coc != NULL) //for each subchild of child
    {
        printf("\t\t\tcase %d:\n", i);
        printf("\t\t\t\t%s();\n", coc->key);
        printf("\t\t\t\tbreak;\n");
        ++i;
        coc = coc->right;
    }
    printf("\t\t\tdefault:\n");
    printf("\t\t\t\tbreak;\n");
    printf("\t\t}\n");
    printf("\t}\n");
    printf("}\n");
}

void call_root()
{
    printf("\troot();\n");
}

int main()
{
    char nname[1024];
    int ptabs;
    int tabs = -1;

    pathtree_t pt;
    tree_t t;
    init_tree(&t);
    init_path(&pt, &t);

    gen_header(); //PART OF GENERATING PROGRAM

    while (fgets(nname, 1024, stdin) != NULL)
    {
        nname[strlen(nname) - 1] = '\0';
        ptabs = tabs;
        for (tabs = 0; nname[tabs] == '\t'; ++tabs)
            ;
        printf("void %s();\n", nname + tabs);
        if (ptabs < tabs) //This node is child to previous Node
        {
            node_t *temp_n = make_node(nname + tabs);
            pathnode_t *temp_pn = pt.head;
            while (temp_pn->next != NULL)
            {
                temp_pn = temp_pn->next;
            }
            temp_pn->next = make_pathnode(temp_n);
            add_child_to_parent(temp_pn->ptr, temp_n);
        }
        else if (ptabs == tabs) //This node is sibling to previous node
        {
            node_t *temp_n = make_node(nname + tabs);
            pathnode_t *temp_pn = pt.head;
            pathnode_t *follow_pn = NULL;
            while (temp_pn->next != NULL)
            {
                follow_pn = temp_pn;
                temp_pn = temp_pn->next;
            }
            temp_pn->ptr = temp_n;
            add_child_to_parent(follow_pn->ptr, temp_n);
        }
        else //case when tabs < ptabs.
        //This node is an sibling to some ancestor of previous node.
        {
            node_t *temp_n = make_node(nname + tabs);
            int counter = -1;
            pathnode_t *temp_pn = pt.head;
            pathnode_t *follow_pn = NULL;
            while (counter < tabs)
            {
                follow_pn = temp_pn;
                temp_pn = temp_pn->next;
                ++counter;
            }

            //Free all nodes next ot temp_pn
            pathnode_t *freer = temp_pn;
            while (temp_pn != NULL)
            {
                temp_pn = temp_pn->next;
                free(freer);
                freer = temp_pn;
            }

            follow_pn->next = make_pathnode(temp_n);
            temp_pn = follow_pn->next;
            temp_pn->next = NULL;
            add_child_to_parent(follow_pn->ptr, temp_n);
        }
    }
    //GPT CONSTRUCTION COMPLETE HERE

    //GENERATING PROGRAM STARTS HERE
    gen_func_r(t.root);
    gen_root(t.root);

    prologue();
    call_root();
    epilogue();
    return 0;
}
