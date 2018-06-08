#include <stdlib.h>

struct node {
    unsigned int client;
    unsigned int subtree_size;
    struct node * ch[2];
} * rt;

static struct node * _create_node() {
    struct node * n = (struct node *) malloc(sizeof(struct node));
    n -> client = 0xffffffff;
    n -> ch[0] = n -> ch[1] = 0;
    n -> subtree_size = 0;
    return n;
}

static int _map_client(struct node ** n, unsigned int address, unsigned int mask, unsigned int client, unsigned int depth) {
    if(!*n) (*n) = _create_node();
    if(depth == mask) {
        if((*n) -> subtree_size) return -1;
        (*n) -> client = client;
        return 0;
    } else {
        if((*n) -> client != 0xffffffff) return -1;
        int res = -1;
        if(address & (1 << depth)) {
            res = _map_client(&((*n) -> ch[1]), address, mask, client, depth + 1);
        } else {
            res = _map_client(&((*n) -> ch[0]), address, mask, client, depth + 1);
        }
        if(res == 0){
            (*n) -> subtree_size += 1;
        }
        return res;
    }
}

int map_client(unsigned int address, unsigned int mask, unsigned int client) {
    if(mask > 32) return -1;
    return _map_client(&rt, address, mask, client, 0);
}

static unsigned int _find_client(struct node * n,unsigned int address, unsigned int mask, unsigned int depth) {
    if(!n) return 0xffffffff;
    if(depth == mask) {
        return n -> client;
    } else {
        if(address & (1 << depth)) {
            return _find_client(n -> ch[1], address, mask, depth + 1);
        } else {
            return _find_client(n -> ch[0], address, mask, depth + 1);
        }
    }
}

unsigned int find_client(unsigned int address, unsigned int mask) {
    if(mask > 32) return 0xffffffff;;
    return _find_client(rt, address, mask,0);
}

static int _unmap_client(struct node ** n,unsigned int address, unsigned int mask, unsigned int depth) {
    if((*n) == NULL) return -1;
    if(depth == mask) {
        if((*n) -> subtree_size != 1) return -1;
        free(*n);
        return 0;
    } else {
        int res = -1;
        if(address & (1 << depth)) {
            res = _unmap_client(&((*n) -> ch[1]), address, mask, depth + 1);
        } else {
            res = _unmap_client(&((*n) -> ch[0]), address, mask, depth + 1);
        }
        if(res == 0) {
            (*n) -> subtree_size -= 1;
        }
        return res;
    }
}

int unmap_client(unsigned int address, unsigned int mask) {
    if(mask > 32) return -1;
    return _unmap_client(&rt, address, mask, 0);
}
