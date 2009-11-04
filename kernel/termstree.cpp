#include "termstree.h"

TermsItem::TermsItem()
{
    p = zero = one = 0;
    term = 0;
    depth = 0;
    flags = 0;
}

TermsItem::TermsItem(TermsItem *p, Term *t) : parent(p)
{
    zero = one = 0;
    depth = p->depth() + 1;
    term = t;
    flags = t? TI_OCCUPIED: 0;
}

void TermsItem::setTerm(Term *t)
{
    term = t;
    if (t)
        enableOccupied();
}

TermsTree::TermsTree(int depth)
{
    root = new TermsItem;
    maxDepth = depth;
}
