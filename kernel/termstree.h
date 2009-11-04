#ifndef TERMSTREE_H
#define TERMSTREE_H

#define TI_OCCUPIED 1

class TermsItem
{
private:
    int depth;
    int flags;
    Term *term;

public:
    TermsItem(TermsItem *p, Term *t = 0);

    inline int depth() { return depth; }

    inline int isOccupied() { return flags & TI_OCCUPIED; }
    inline int enableOccupied() { flags |= TI_OCCUPIED; }
    inline int disableOccupied() { flags &= ~TI_OCCUPIED; }

    inline Term *getTerm() { return term; }
    void setTerm(Term *t);

    TermsItem *parent;
    TermsItem *zero;
    TermsItem *one;
};

class TermsTree
{
private:
    TermsItem *root;
    int maxDepth;
public:
    TermsTree(int depth);
};

#endif // TERMSTREE_H
