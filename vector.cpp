// constructing vectors
#include <iostream>
#include <vector>



typedef struct node {
    struct node *parent;
    int x, y;
    float f, g, h;
} node;

node* new_node(int x, int y, double g, double h){
    node* a = (node*) malloc(sizeof(node));
    
    a->x = x;
    a->y = y;
    
    a->g = g;
    a->h = h;

    a->f = a->g + a->h;
    
    return a;
}


int main ()
{
  
  std::vector<node*> vec;

  for(int i = 0; i < 10; i++){
    vec.push_back( new_node( i,i,i,i ) );
  }
  
  for (std::vector<node*>::iterator it = vec.begin(); it != vec.end(); ++it)
    std::cout << ' ' << (*it)->f;
  std::cout << '\n';

  return 0;
}