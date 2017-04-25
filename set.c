//
// Created by aaron on 17-4-25.
//

#include "set.h"

Prod p[PROD_NUM];
int times = 0;

// Construct production by hand
void construct_production() {
    /*
     * A <- E'
     * B <- T'
     * a <- id
     */
    /*
    // Production 1
     strcpy(p[0].head, "E");
     strcpy(p[0].body[0], "TA");
     p[0].n = 1;

     // Production 2
     strcpy(p[1].head, "A");
     strcpy(p[1].body[0], "+TA");
     strcpy(p[1].body[1], "0");
     p[1].n = 2;

     // Production 3
     strcpy(p[2].head, "T");
     strcpy(p[2].body[0], "FB");
     p[2].n = 1;

     // Production 4
     strcpy(p[3].head, "B");
     strcpy(p[3].body[0], "*FB");
     strcpy(p[3].body[1], "0");
     p[3].n = 2;

     // Production 5
     strcpy(p[4].head, "F");
     strcpy(p[4].body[0], "(E)");
     strcpy(p[4].body[1], "a");
     p[4].n = 2;
     */
    strcpy(p[0].head, "S");
    strcpy(p[0].body[0], "AB");
    strcpy(p[0].body[1], "bC");
    p[0].n = 2;

    strcpy(p[1].head, "A");
    strcpy(p[1].body[0], "0");
    strcpy(p[1].body[1], "b");
    p[1].n = 2;

    strcpy(p[2].head, "B");
    strcpy(p[2].body[0], "0");
    strcpy(p[2].body[1], "aD");
    p[2].n = 2;

    strcpy(p[3].head, "C");
    strcpy(p[3].body[0], "AD");
    strcpy(p[3].body[1], "b");
    p[3].n = 2;

    strcpy(p[4].head, "D");
    strcpy(p[4].body[0], "aS");
    strcpy(p[4].body[1], "c");
    p[4].n = 2;
}

// Print all productions
void print_production() {
    int i, j;

    for (i = 0; i < 5; i++) {
        printf("%s -> ", p[i].head);
        for (j = 0; j < p[i].n; j++) {
            printf("%s", p[i].body[j]);
            if (j + 1 != p[i].n) {
                printf(" | ");
            }
        }
        printf("\n");
    }
}

int is_terminal(char ch) {
    if (ch <= 'Z' && ch >= 'A')
        return 0;
    return 1;
}

// return 1 if x∈f, else return 0
int has_elem(Set f, char x) {
    int i;

    for (i = 0; i < f.n; i++)
        if (f.data[i] == x)
            return 1;
    return 0;
}

// Find Production preceding with X and return that production, such as X->AB|CD
Prod *find_production(char X) {
    int i;
    for (i = 0; i < PROD_NUM; i++) {
        if (p[i].head[0] == X) {
            return &p[i];
        }
    }
    return NULL;
}

// initialize first/follow
void init_set(Set *f) {
    f->n = 0;
}

// Merge two FIRST/FOLLOW sets, saved to f1
void merge_first_follow(Set *f1, const Set *f2) {
    int i;

    for (i = 0; i < f2->n; i++) {   // compare f2 with f1
        if (!has_elem(*f1, f2->data[i])) {
            f1->data[f1->n] = f2->data[i];
            f1->n++;
        }
    }
    f1->data[f1->n] = '\0';
}

// Remove empty in set  (for computing FOLLOW)
void remove_empty(Set *set) {
    int i, j;
    for (i = 0; i < set->n; i++) {
        if ('0' == set->data[i]) {  // if 0, then move elements in set ahead
            for (j = i; j < set->n; j++) {
                set->data[j] = set->data[j + 1];
            }
            set->n--;
        }
    }
    if (has_elem(*set, '0')) {
        remove_empty(set);
    }
}

// Remove duplicated empty in set(for not showing duplicated ?)
void remove_dup_empty(Set *set) {    //È¥³ýÖØžŽµÄ¿Õ£¬œöÊ£Ò»žö
    int i, j, emp_count = 0;
    for (i = 0; i < set->n; i++) {
        if (set->data[i] == '0') {  // if find empty
            emp_count++;
            if (emp_count > 1) {    // duplicated emtpy, then remove
                for (j = i; j < set->n; j++) {
                    set->data[j] = set->data[j + 1];
                }
                set->n--;
            }
        }
    }
}

Set compute_first(char X) {
    Set first_x;
    init_set(&first_x);
    int i, j;

    // X is terminal,then FIRST(X) = {X}
    if (is_terminal(X)) {
        first_x.data[first_x.n++] = X;
        first_x.data[first_x.n] = '\0';
    }
        // if X is nonterminal, compute FIRST(X) recursively
    else {
        Prod *p = find_production(X);   // find the corresponding production

        for (i = 0; i < p->n; i++) {    // loop each body
            for (j = 0; j < strlen(p->body[i]); j++) {  // loop each body's each character
                Set temp = compute_first(p->body[i][j]);    // body[i], character j

                if (j == strlen(p->body[i]) - 1) {
                    merge_first_follow(&first_x, &temp);
                } else {
                    Set temp2 = temp;
                    remove_empty(&temp2);
                    merge_first_follow(&first_x, &temp2);
                }
                if (has_elem(temp, '0')) continue;
                else break;
            }
        }
        remove_dup_empty(&first_x);
    }
    return first_x;
}

Set compute_follow(char B) {
    if (times++ > MAX_REC) {
        Set f;
        init_set(&f);
        return f;
    }
    if (is_terminal(B)) {
        fprintf(stderr, "Cann't compute Follow for terminal!\n");
        exit(1);
    }
    Set follow_B;
    int i, j, k;
    init_set(&follow_B);

    Prod *ptr = find_production(B);
    // Case 1: Place end mark '#' in follow(B) if B is start symbol
    if (ptr != NULL && ptr->head[0] == B && ptr == &p[0]) {
        follow_B.data[follow_B.n++] = '#';
        follow_B.data[follow_B.n] = '\0';
    }

    for (i = 0; i < PROD_NUM; i++) {    // loop each production
        for (j = 0; j < p[i].n; j++) {  // loop each production's body
            for (k = 0; p[i].body[j][k] != '\0'; k++) { // loop each production's each body's each character
                // Case 2: A -> αBβ
                if (p[i].body[j][k] == B && p[i].body[j][k + 1] != '\0') {
                    // everything in FIRST(β) is in FOLLOW(B)
                    Set first = compute_first(p[i].body[j][k + 1]);   // First(ŠÂ)-'0' -> FOLLOW(B)
                    remove_empty(&first);
                    merge_first_follow(&follow_B, &first);

                    // if β->0, then everything in FOLLOW(β) is in FOLLOW(B)
                    if (has_elem(compute_first(p[i].body[j][k + 1]), '0')) {    // if β->0
                        Set follow = compute_follow(p[i].body[j][k + 1]); // FOLLOW(β)
                        merge_first_follow(&follow_B, &follow);
                    }
                }
                // Case 3: A->αB, then everything in FOLLOW(A) is in FOLLOW(B)
                // Attention: A -> +TA, So When compute FOLLOW(A), infinate loop would occur!
                if (p[i].body[j][k] == B && p[i].body[j][k + 1] == '\0' && p[i].head[0] != B) {
                    Set follow_a = compute_follow(p[i].head[0]); // compute FOLLOW(A)
                    merge_first_follow(&follow_B, &follow_a);   // add FOLLOW(A) to FOLLOW(B)
                }
            }
        }
    }
    return follow_B;
}

Set compute_select(const Prod *p, int prod_n, int body_n) {
    int i;
    Set first;
    init_set(&first);

    for (i = 0; i < strlen(p[prod_n].body[body_n]); i++) {  // loop each character
        Set temp = compute_first(p[prod_n].body[body_n][i]);
        if (has_elem(temp, '0')) {
            if (i == strlen(p->body[i]) - 1) {
                merge_first_follow(&first, &temp);
            } else {
                Set temp2 = temp;
                remove_empty(&temp2);
                merge_first_follow(&first, &temp2);
            }
        } else {
            merge_first_follow(&first, &temp);
            break;
        }
    }

    // if α!->0, Select(A->α) = FIRST(α)
    if (!has_elem(first, '0') && p[prod_n].body[body_n][0] != '0')
        return first;
        // if α->0, Select(A->α) = {FIRST(α)-0} ∪ FOLLOW(A)
    else {
        remove_empty(&first);   // {FIRST(α)-0}
        Set follow = compute_follow(p[prod_n].head[0]); //FOLLOW(A)
        merge_first_follow(&first, &follow);
        return first;
    }
}

// Print formatted set
void print_set(Set *set) {
    int i;
    remove_dup_empty(set);
    printf("{");
    for (i = 0; i < set->n; i++) {
        if (i == set->n - 1) {
            if (set->data[i] == '0')
                printf("%s", "ɛ");
            else
                printf("%c", set->data[i]);
        } else {
            if (set->data[i] == '0')
                printf("%s, ", "ɛ");
            else
                printf("%c, ", set->data[i]);
        }
    }
    printf("}\n");
}

int is_disjoint(const Set *set1, const Set *set2) {
    int i, j;
    for (i = 0; i < set1->n; i++) {
        for (j = 0; j < set2->n; j++) {
            if (set2->data[j] == set1->data[i])
                return 0;
        }
    }
    return 1;
}

int is_LL1(Prod p[]) {
    //The first two conditions are equivalent to the statement that FIRST(a) and
    //FIRST(β) are disjoint sets.

    //The third condition is equivalent to stating that if
    //0 is in FIRST(β) , then FIRsT(α) and FOLLOW(A) are disjoint sets, and likewise
    //if 0 is in FIRsT(a) .

    // Condition 1: disjoint sets
    int i, j, k;
    for (i = 0; i < PROD_NUM; i++) {  // loop each production
        for (j = 0; j < p[i].n; j++) {  // loop each body
            for (k = j + 1; k < p[i].n; k++) {    // compare each body
                Set first1 = compute_first(p[i].body[j][0]);
                Set first2 = compute_first(p[i].body[k][0]);
                if (!is_disjoint(&first1, &first2)) {
                    return 0;
                }

            }
        }
    }
    // Condition 2

    for (i = 0; i < PROD_NUM; i++) {  // loop each production
        Set follow = compute_follow(p[i].head[0]); // FOLLOW(A)
        for (j = 0; j < p[i].n; j++) {  // loop each body
            Set first1 = compute_first(p[i].body[j][0]);    // FIRST(ŠÁ)
            if (has_elem(first1, '0')) {    // 0 ¡Ê FIRST(ŠÁ)
                for (k = 0; k < p[i].n; k++) {  // compute all FIRST(ŠÂ)
                    if (k != j) {
                        Set first_b = compute_first(p[i].body[k][0]);
                        if (!is_disjoint(&first_b, &follow)) {   // FIRST(ŠÂ) and FOLLOW(A) are disjoint sets
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}

int is_LL1_s(Prod p[]) {
    int i, j, k;
    for (i = 0; i < PROD_NUM; i++) {  // loop each production
        for (j = 0; j < p[i].n; j++) {  // loop each body
            for (k = 0; k < p[i].n; k++) {
                if (k != j) {
                    Set select1 = compute_select(p, i, j);    // p[i].body[j]
                    Set select2 = compute_select(p, i, k);   // p[i].body[k]
                    if (!is_disjoint(&select1, &select2)) { // SELECT(A->α)∩SELECT(A->β) = ∅
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

void main(int argc, char **argv) {
    /* Compute First */
    Set first, select;
    init_set(&first);
    init_set(&select);
    int i, j;

    construct_production();
    print_production();

    /* Compute first */
    printf("===================\n");
    for (i = 0; i < 5; i++) {
        init_set(&first);
        first = compute_first(p[i].head[0]);
        printf("First(%c) = ", p[i].head[0]);
        print_set(&first);
    }

    /* Compute follow */
    printf("===================\n");
    for (i = 0; i < 5; i++) {
        Set follow;
        init_set(&follow);
        follow = compute_follow(p[i].head[0]);
        printf("Follow(%c) = ", p[i].head[0]);
        print_set(&follow);
        times = 0;    // avoid infinite loop
    }

    /* compute select */
    printf("===================\n");
    for (i = 0; i < PROD_NUM; i++) {
        for (j = 0; j < p[i].n; j++) {
            select = compute_select(p, i, j);
            printf("Select(%c->%s) = ", p[i].head[0], p[i].body[j]);
            print_set(&select);
            times = 0;
        }
    }

    /* Judge LL(1) */
    printf("===================\n");
    if (is_LL1_s(p))
        printf("Is LL(1)!\n");
    else
        printf("Is not LL(1)!\n");
}
