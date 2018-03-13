#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>

#define PI 3.1415

struct matrix {
  int entries;
  float array[4][40];
};

struct matrix* make_new(){
  struct matrix * b = malloc(sizeof(struct matrix));
  (*b).entries = 0;
  return b;
}

void add_single(struct matrix * b, float x, float y, float z, float u){
  b->array[0][b->entries] = x;
  b->array[1][b->entries] = y;
  b->array[2][b->entries] = z;
  b->array[3][b->entries] = u;
  b->entries += 1;
}



void add_entry(struct matrix * b, float x0, float y0, float z0, float b0, float x1, float y1, float z1, float b1){
  b->array[0][b->entries] = x0;
  b->array[1][b->entries] = y0;
  b->array[2][b->entries] = z0;
  b->array[3][b->entries] = b0;
  b->array[0][b->entries + 1] = x1;
  b->array[1][b->entries + 1] = y1;
  b->array[2][b->entries + 1] = z1;
  b->array[3][b->entries + 1] = b1;
  b->entries += 2;
}

struct matrix* make_identity(){
  struct matrix *b = make_new();
  add_entry(b, 1, 0, 0, 0, 0, 1, 0, 0);
  add_entry(b, 0, 0, 1, 0, 0, 0, 0, 1);
}

void print_matrix(struct matrix * b){
  int i, j;
  for(i = 0; i < 4; i++){
    for(j = 0; j < b->entries; j++){
      printf("%f ", b->array[i][j]);      
    }
    printf("\n");
  }
}

void multiply(struct matrix * a, struct matrix * b){
  int counter = b->entries;
  float w = 0;
  float x = 0;
  float y = 0;
  float z = 0;

  int i;
  for(i = 0; i < counter; i++){
    w = a->array[0][0] * b->array[0][i] + a->array[0][1] * b->array[1][i] + a->array[0][2] * b->array[2][i] + a->array[0][3] * b->array[3][i];
    x = a->array[1][0] * b->array[0][i] + a->array[1][1] * b->array[1][i] + a->array[1][2] * b->array[2][i] + a->array[1][3] * b->array[3][i];
    y = a->array[2][0] * b->array[0][i] + a->array[2][1] * b->array[1][i] + a->array[2][2] * b->array[2][i] + a->array[2][3] * b->array[3][i];
    z = a->array[3][0] * b->array[0][i] + a->array[3][1] * b->array[1][i] + a->array[3][2] * b->array[2][i] + a->array[3][3] * b->array[3][i];
    b->array[0][i] = w;
    b->array[1][i] = x;
    b->array[2][i] = y;
    b->array[3][i] = z;    
  }
}

void linemaker(char image[500][500], int x, int y, int ex, int ey){
  float dx = ex - x;
  float dy = ey - y;
  if( (dx < 0 && dy < 0) || (dx > 0 && dy < 0) || (dx == 0 && dy < 0) || (dy == 0 && dx < 0)){
    float tx = x;
    float ty = y;
    x = ex;
    y = ey;
    ex = tx;
    ey = ty;
    dx = ex - x;
    dy = ey - y;
  }
  float c = dx * y - dy * x;
  if(dy > 0 && dx < 0){
    float d =  2 * dy * (x + 1) - dx * (2 * y + 1) + 2 * c;
    if(dx + dy <= 0){
       while(x >= ex){
	 image[x][y] = 'r';
	 if(d < 0){
	   d -= 2 * dx;
	   y++;
	 }
	 d -= 2 * dy;
	 x--;
       }
    }
    else{
      while(y <= ey){
	 image[x][y] = 'r';
	 if(d > 0){
	   d -= 2 * dy;
	   x--;
	 }
	 d -= 2 * dx;
	 y++;
       }
    }
  } 
  else if(dx >= dy){
    float d =  2 * dy * (x + 1) - dx * (2 * y + 1) + 2 * c;   
    while(x <= ex){
      image[x][y] = 'r';
      if(d > 0){
	d -= 2 * dx;
	y++;
      }
      d += 2 * dy;
      x++;
    }
  }  
  else if(dy > dx){
    float d =  dy * (2 * x + 1) - dx * (2 * y) + 2 * c;   
    while(y <= ey){
      image[x][y] = 'r';
      if(d < 0){
	d += 2 * dy;
	x++;
      }
      d -= 2 * dx;
      y++;
    }
  }
}

void edgemaker(struct matrix * a, char image[500][500]){
  int counter = a->entries;
  int i;
  for(i = 0; i < counter; i+=2){
    linemaker(image, (int)round(a->array[0][i]), (int)round(a->array[1][i]), (int)round(a->array[0][i+1]), (int)round(a->array[1][i+1]));
  }
}


struct matrix * translation(float x, float y, float z){
  struct matrix * b = make_identity();
  b->array[0][3] = x;
  b->array[1][3] = y;
  b->array[2][3] = z;
  return b;
}

struct matrix * scale(float x, float y, float z){
  struct matrix * b = make_identity();
  b->array[0][0] = x;
  b->array[1][1] = y;
  b->array[2][2] = z;
  return b;
}

struct matrix * z_rotation(double x){ //x-y
  struct matrix * b = make_identity();
  
  b->array[0][0] = cos(x * PI / 180);
  b->array[0][1] = -1 * sin(x * PI / 180);
  b->array[1][0] = sin(x * PI / 180);
  b->array[1][1] = cos(x * PI / 180);
  
  return b;
}

struct matrix * y_rotation(double x){ //z-x
  struct matrix * b = make_identity();
  
  b->array[2][2] = cos(x * PI / 180);
  b->array[2][0] = -1 * sin(x * PI / 180);
  b->array[0][2] = sin(x * PI / 180);
  b->array[0][0] = cos(x * PI / 180);
  
  return b;
}

struct matrix * x_rotation(double x){ //y-z
  struct matrix * b = make_identity();
  
  b->array[1][1] = cos(x * PI / 180);
  b->array[1][2] = -1 * sin(x * PI / 180);
  b->array[2][1] = sin(x * PI / 180);
  b->array[2][2] = cos(x * PI / 180);
  
  return b;
}

void save(char image[500][500], char* file){
  int b = open(file, O_CREAT | O_WRONLY | O_APPEND, 888);
  int i, j;
  write(b, "P3 500 500 255", sizeof("P3 500 500 255"));
  for(i = 0; i < 500; i++){
    for(j = 0; j < 500; j++){
      if(image[i][j] == 'g')
	write(b, "0 255 0", sizeof("0 255 0"));
      if(image[i][j] == 'r')
	write(b, "255 0 0", sizeof("255 0 0"));
    }
  }
}

void circle(struct matrix * edges, double x, double y, double z, double r){
  double t;
  double xx, yy;
  for(t = 0; t < 360; t += 1){
    xx = x + r * cos(t * PI / 180);
    yy = y + r * sin(t * PI / 180);
    add_entry(edges, xx, yy, z, 1, x + r * cos((t + 1) * PI / 180) , y + r * sin((t + 1) * PI / 180), z, 1);
  }
}

void hermite(struct matrix * edges, double x0, double y0, double x1, double y1, double rx0, double ry0, double rx1, double ry1){
  struct matrix * herm = make_new();
  add_entry(herm, 2, -3, 0, 1, -1, 3, 0, 0);
  add_entry(herm, 1, -2, 1, 0, 1, -1, 0, 0);

  
  struct matrix * xcoeffs = make_new();
  add_single(xcoeffs, x0, x1, rx0, rx1);
  multiply(herm, xcoeffs);
  double xa = xcoeffs->array[0][0];
  double xb = xcoeffs->array[1][0];
  double xc = xcoeffs->array[2][0];
  double xd = xcoeffs->array[3][0];

  
  struct matrix * ycoeffs = make_new();
  add_single(ycoeffs, y0, y1, ry0, ry1);
  multiply(herm, ycoeffs);
  double ya = ycoeffs->array[0][0];
  double yb = ycoeffs->array[1][0];
  double yc = ycoeffs->array[2][0];
  double yd = ycoeffs->array[3][0];


  double X, Y;
  double t;

  for(t = 0; t < 1; t+= .01){
    X = xa * t * t * t + xb * t * t + xc * t + xd;
    Y = ya * t * t * t + yb * t * t + yc * t + yd;

    printf("X: %f Y: %f \n", X, Y);
    
    add_entry(edges, X, Y, 0, 1,
	      xa * (t + .05) * (t + .05) * (t + .05) + xb * (t + .05) * (t + .05) + xc * (t + .05) + xd,
	      ya * (t + .05) * (t + .05) * (t + .05) + yb * (t + .05) * (t + .05) + yc * (t + .05) + yd, 0, 1);
   
  }
  

}

void bezier(struct matrix * edges, double x0, double y0, double x1, double y1, double x2, double y2, double x3, double y3){
  struct matrix * bez = make_new();
  add_entry(bez, -1, 3, -3, 1, 3, -6, 3, 0);
  add_entry(bez, -3, 3, 0, 0, 1, 0, 0, 0);

  
  struct matrix * xcoeffs = make_new();
  add_single(xcoeffs, x0, x1, x2, x3);
  multiply(bez, xcoeffs);
  double xa = xcoeffs->array[0][0];
  double xb = xcoeffs->array[1][0];
  double xc = xcoeffs->array[2][0];
  double xd = xcoeffs->array[3][0];

  
  struct matrix * ycoeffs = make_new();
  add_single(ycoeffs, y0, y1, y2, y3);
  multiply(bez, ycoeffs);
  double ya = ycoeffs->array[0][0];
  double yb = ycoeffs->array[1][0];
  double yc = ycoeffs->array[2][0];
  double yd = ycoeffs->array[3][0];

  
  double X, Y;
  double t;

  for(t = 0; t < 1; t+= .01){
    X = xa * t * t * t + xb * t * t + xc * t + xd;
    Y = ya * t * t * t + yb * t * t + yc * t + yd;

    add_entry(edges, X, Y, 0, 1,
	      xa * (t + .05) * (t + .05) * (t + .05) + xb * (t + .05) * (t + .05) + xc * (t + .05) + xd,
	      ya * (t + .05) * (t + .05) * (t + .05) + yb * (t + .05) * (t + .05) + yc * (t + .05) + yd, 0, 1);

  }  



}



void main(){
  char image[500][500];
  int i;
  int j;
  for(i = 0; i < 500; i++){
    for(j = 0; j < 500; j++){
      image[i][j] = 'g';
    }
  }

  struct matrix * transformations = make_identity();
  struct matrix * edges = make_new();

  char input[100];
  int red = open("input.s", O_RDONLY, 0);
  read(red, input, sizeof(input));

  char* token;

  
  token = strtok(input, " \n");
  while(token != NULL){
    if(strcmp(token, "line") == 0){
      token = strtok(NULL, " \n");
      int a = atoi(token);
      token = strtok(NULL, " \n");
      int b = atoi(token);
      token = strtok(NULL, " \n");
      int c = atoi(token);

      token = strtok(NULL, " \n");
      int d = atoi(token);
      token = strtok(NULL, " \n");
      int e = atoi(token);
      token = strtok(NULL, " \n");
      int f = atoi(token);

      add_entry(edges, a, b, c, 1, d, e, f, 1);
    }
    else if(strcmp(token, "ident") == 0){
      transformations = make_identity();
    }
    else if(strcmp(token, "scale") == 0){
      token = strtok(NULL, " \n");
      int m = atoi(token);
      token = strtok(NULL, " \n");
      int n = atoi(token);
      token = strtok(NULL, " \n");
      int o = atoi(token);
      struct matrix * asd = scale(m, n, o);
      multiply(asd, transformations);
    }
    else if(strcmp(token, "move") == 0){
      token = strtok(NULL, " \n");
      int m = atoi(token);
      token = strtok(NULL, " \n");
      int n = atoi(token);
      token = strtok(NULL, " \n");
      int o = atoi(token);
      struct matrix * asd = translation(m, n, o);
      multiply(asd, transformations);      
    }
    else if(strcmp(token, "rotate") == 0){
      token = strtok(NULL, " \n");
      if(strcmp(token, "z") == 0){
	token = strtok(NULL, " \n");
	int m = atoi(token);
	struct matrix * asd = z_rotation(m);
	multiply(asd, transformations);
      }
      else if(strcmp(token, "y") == 0){
	token = strtok(NULL, " \n");
	int m = atoi(token);
	struct matrix * asd = y_rotation(m);
	multiply(asd, transformations);
      }
      else if(strcmp(token, "x") == 0){
	token = strtok(NULL, " \n");
	int m = atoi(token);
	struct matrix * asd = x_rotation(m);
	multiply(asd, transformations);
      }
    }
    else if(strcmp(token, "apply") == 0){
      multiply(transformations, edges);
    }
    else if(strcmp(token, "display") == 0){
      token = strtok(NULL, " \n");
      edgemaker(edges, image);
      save(image, token); 
    }
    else if(strcmp(token, "save") == 0){
      token = strtok(NULL, " \n");
      edgemaker(edges, image);
      save(image, token);      
    }
    else if(strcmp(token, "circle") == 0){
      token = strtok(NULL, " \n");
      int x = atof(token);
      token = strtok(NULL, " \n");
      int y = atof(token);
      token = strtok(NULL, " \n");
      int z = atof(token);
      token = strtok(NULL, " \n");
      int r = atof(token);
      circle(edges, x, y, z, r);
    }
    else if(strcmp(token, "hermite") == 0){
      token = strtok(NULL, " \n");
      int x0 = atoi(token);
      token = strtok(NULL, " \n");
      int y0 = atoi(token);
      token = strtok(NULL, " \n");
      int x1 = atoi(token);
      token = strtok(NULL, " \n");
      int y1 = atoi(token);
      token = strtok(NULL, " \n");
      int rx0 = atof(token);
      token = strtok(NULL, " \n");
      int ry0 = atof(token);
      token = strtok(NULL, " \n");
      int rx1 = atof(token);
      token = strtok(NULL, " \n");
      int ry1 = atof(token);
      hermite(edges, x0, y0, x1, y1, rx0, ry0, rx1, ry1);
    }
    else if(strcmp(token, "bezier") == 0){
      token = strtok(NULL, " \n");
      int x0 = atoi(token);
      token = strtok(NULL, " \n");
      int y0 = atoi(token);
      token = strtok(NULL, " \n");
      int x1 = atoi(token);
      token = strtok(NULL, " \n");
      int y1 = atoi(token);
      token = strtok(NULL, " \n");
      int x2 = atof(token);
      token = strtok(NULL, " \n");
      int y2 = atof(token);
      token = strtok(NULL, " \n");
      int x3 = atof(token);
      token = strtok(NULL, " \n");
      int y3 = atof(token);
      bezier(edges, x0, y0, x1, y1, x2, y2, x3, y3);
    }
    token = strtok(NULL, " \n");
  }
 
  
}
