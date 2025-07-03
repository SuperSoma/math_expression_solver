#include <stdio.h>
#include <stdlib.h>

enum {Num, Add, Sub, Mul, Div, Opn, Cls, Tkn};

typedef struct element {
  int type;
  int value;
  struct element *next;
} element;

typedef struct {
  element *head;
} stack;

void push(stack *s, element *e) {
  e->next = NULL;
  if (!s->head) {
    s->head = e;
    return;
  }

  element *oldHead = s->head;
  e->next = oldHead;
  s->head = e;
}

void pop(stack *s, element **e) {
  if (!s->head) {
    *e = NULL;
    return;
  }

  *e = s->head;

  if (!s->head->next) {
    s->head = NULL;
    return;
  }

  s->head = s->head->next;
}

void peek(stack *s, element **e) {
  if (!s->head) {
    *e = NULL;
    return;
  }

  *e = s->head;
}

int main(void) {
  char buf[1024];
  printf("Please enter a mathmatical expression that uses only integers: ");
  fgets(buf, 1024, stdin);

  char *token = buf;

  stack output;
  stack operators;
  output.head = NULL;
  operators.head = NULL;

  //convert the expression to a postfix notation
  while (*token != '\n') {
    if (*token >= '0' && *token <= '9') {
      element *number = malloc(sizeof(element));
      number->type = Num;
      number->value = 0;
      do {
        number->value = (number->value * 10) + (*token - '0');
        token++;
      } while (*token >= '0' && *token <= '9');
      push(&output, number);
    } else if (*token == '+' || *token == '-' || *token == '*' || *token == '/' || *token == '(' || *token == ')') {
      element *operator = malloc(sizeof(element));
      operator->value = (int)*token;
      operator->type = Tkn;
      if (*token == '(') {
        push(&operators, operator);
        token++;
        continue;
      }

      if (*token == ')') {
        free(operator);
        element *temp;
        do {
          pop(&operators, &temp);
          if (!temp) {
            printf("ERROR WITH CLOSING PARENTHESES\n");
            return 1;
          }

          if (temp->value != '(') {
            push(&output, temp);
          }
        } while (temp->value != '(');
        token++;
        continue;
      }

      if (*token == '+' || *token == '-' || *token == '*' || *token == '/') {
        int precedence = (*token == '+' || *token == '-') ? 1 : 2;
        element *checkTop;
        peek(&operators, &checkTop);
        if (!checkTop) {
          push(&operators, operator);
          token++;
          continue;
        }
        int topPrecedence = (checkTop->value == '+' || checkTop->value == '-') ? 1 : (checkTop->value == '(' ? 0 : 2);

        if (topPrecedence >= precedence) {
          pop(&operators, &checkTop);
          push(&output, checkTop);
          push(&operators, operator);
        } else {
          push(&operators, operator);
        }
        token++;
        continue;
      }
    } else {
      token++;
    }
  }

  element *checkTop;
  pop(&operators, &checkTop);

  while(checkTop) {
    push(&output, checkTop);
    pop(&operators, &checkTop);
  }

  stack rpn;
  rpn.head = NULL;

  peek(&output, &checkTop);
  while(checkTop) {
    pop(&output, &checkTop);
    push(&rpn, checkTop); //this is to reverse the stack so everything is in the proper order
    peek(&output, &checkTop);
  }

  //Solve the Reverse Polish Notation
  stack numberStack;
  numberStack.head = NULL;
  pop(&rpn, &checkTop);
  while(checkTop) {
    if (checkTop->type == Num) {
      push(&numberStack, checkTop);
    } else {
      element *a, *b;
      element *result = malloc(sizeof(element));
      result->type = Num;
      pop(&numberStack, &b);
      pop(&numberStack, &a);

      int *r = &(result->value); //lazy

      switch(checkTop->value) {
        case '+': *r = a->value + b->value; break;
        case '-': *r = a->value - b->value; break;
        case '*': *r = a->value * b->value; break;
        case '/': *r = a->value / b->value; break;
      }

      free(a);
      free(b);

      push(&numberStack, result);
    }
    pop(&rpn, &checkTop);
  }

  peek(&numberStack, &checkTop);
  token = buf;
  while (*token != '\n') {
    printf("%c", *token);
    token++;
  }

  printf(" = %d\n", checkTop->value);

  return 0;
}