#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define align4(x) (((((x)-1)>>2)<<2)+4)
#define BLOCK_SIZE 20

typedef struct s_block * t_block;

struct s_block {
    size_t size;
    struct s_block *next;
    struct s_block *prev;
    int free;
    void *ptr;
    /* A pointer to the allocated block */
    char data[1];
};

void *base = NULL;

t_block extend_heap(t_block, size_t);
t_block find_block(t_block *, size_t);
void split_block(t_block, size_t);
void *malloco(size_t);
int valid_addr(void *p);
t_block fusion(t_block b);
void freep(void *p);

int main() {

    char *str;
    str = (char *) malloco(12);

    printf("el str se malloqueo en %u\n", str);
    printf("el sbrk en %u\n", sbrk(0));

    freep(str);

    printf("el str liberorororo en %u\n", str);
    printf("el sbrk en %u\n", sbrk(0));

    char *str2;
    str2 = (char *) malloco(12);

    printf("el str2 se malloqueo en %u\n", str2);
    printf("el sbrk en %u\n", sbrk(0));


    printf("\n///////////////////////////////////////\n\n");

    char *str3;
    str3 = (char *) malloc(12);

    printf("el str3 se malloc posta en %u\n", str3);
    printf("el sbrk4 en %u\n", sbrk(0));

    free(str3);

    printf("el str3 libero posta en %u\n", str3);
    printf("el sbrk en %u\n", sbrk(0));

    char *str4;
    str4 = (char *) malloc(12);

    printf("el str4 se malloc posta en %u\n", str4);
    printf("el sbrk4 en %u\n", sbrk(0));

    free(str4);

    return 0;
}

t_block find_block(t_block *last, size_t size){

    t_block bloque = base;

    while (bloque && !(bloque -> free && bloque -> size >= size )) {
        *last = bloque;
        bloque = bloque -> next;
    }

    return (bloque);
}

t_block extend_heap(t_block last, size_t s){

    int sb;
    t_block b;
    b = sbrk (0);
    sb = (int) sbrk ( BLOCK_SIZE + s);

    if(sb < 0){
        return (NULL );
    }

    b->size = s;
    b->next = NULL;
    b->prev = last;
    b->ptr = b->data;

    if (last){
        last->next = b;
    }

    b->free = 0;

    return (b);
}

void split_block(t_block b, size_t s){

    t_block new;

    new = ( t_block )(b->data + s);
    new->size = b->size - s - BLOCK_SIZE ;
    new->next = b->next;
    new->prev = b;
    new->free = 1;
    new->ptr = new->data;
    b->size = s;
    b->next = new;

    if (new ->next){
        new ->next ->prev = new;
    }

}

void *malloco(size_t size){

    t_block b, last;
    size_t s;

    s = align4(size);

    if(base){

        /* First find a block */
        last = base;
        b = find_block(& last, s);

        if(b){

            /* can we split */
            if ((b->size - s) >= ( BLOCK_SIZE + 4)){
                split_block (b,s);
            }

            b->free = 0;
        }
        else{

            /* No fitting block , extend the heap */
            b = extend_heap (last ,s);

            if(!b){
                return(NULL);
            }
        }
    }
    else{

        /* first time */
        b = extend_heap (NULL, s);

        if(!b){
            return (NULL);
        }

        base = b;
    }

    return(b->data);
}

//Get the block from and addr
t_block get_block (void *p){

    char *tmp;
    tmp = p;

    return (p = tmp -= BLOCK_SIZE );

}

int valid_addr(void *p){

    if (base)
    {
        if ( p>base && p< sbrk (0))
        {
            return ( p == (get_block(p))-> ptr );
        }
    }

    return (0);
}

void freep(void *p){

    t_block b;

    if(valid_addr (p))
    {
        b = get_block (p);
        b -> free = 1;

        /* fusion with previous if possible */
        if(b->prev && b->prev ->free){
            b = fusion (b->prev );
        }

        /* then fusion with next */
        if (b->next){
            fusion (b);
        }
        else
        {
            /* free the end of the heap */
            if (b->prev){
                b -> prev -> next = NULL;
            }

            else{
                /* No more block !*/
                base = NULL;
            }

            brk(b);
        }
    }
}

t_block fusion(t_block b){

    if (b->next && b->next ->free ){

        b->size += BLOCK_SIZE + b->next ->size;
        b->next = b->next->next;

        if (b->next){
            b->next ->prev = b;
        }
    }

    return (b);
}