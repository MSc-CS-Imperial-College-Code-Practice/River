#include <iostream>
#include <fstream>
#include <cstring>
#include <cassert>

using namespace std;

#include "river.h"

/* You are pre-supplied with the functions below. Add your own 
   function definitions to the end of this file. */

/* internal helper function which allocates a dynamic 2D array */
char **allocate_2D_array(int rows, int columns) {
  char **m = new (nothrow) char *[rows];
  assert(m);
  for (int r=0; r<rows; r++) {
    m[r] = new (nothrow) char[columns];
    assert(m[r]);
  }
  return m;
}

/* internal helper function which deallocates a dynamic 2D array */
void deallocate_2D_array(char **m, int rows) {
  for (int r=0; r<rows; r++)
    delete [] m[r];
  delete [] m;
}

/* pre-supplied function which creates an empty ASCII-art scene */
char **create_scene() {
  char **scene = allocate_2D_array(SCENE_HEIGHT, SCENE_WIDTH);

  for (int i=0; i<SCENE_HEIGHT; i++) 
    for (int j=0; j<SCENE_WIDTH; j++)
      scene[i][j] = ' ';

  return scene;
}

/* pre-supplied function which frees up memory allocated for an ASCII-art scene */
void destroy_scene(char **scene) {
  deallocate_2D_array(scene, SCENE_HEIGHT);
}

/* pre-supplied function which displays an ASCII-art scene */
void print_scene(char **scene) {
  for (int i=0; i<SCENE_HEIGHT; i++) {
    for (int j=0; j<SCENE_WIDTH; j++)
      cout << scene[i][j];
    cout << endl;
  }
}

/* helper function which removes carriage returns and newlines from strings */
void filter(char *line) {
  while (*line) {
    if (*line >= ' ')
      line++;
    else 
      *line = '\0';
  }
}

/* pre-supplied function which inserts an ASCII-art drawing stored in a file
   into a given ASCII-art scene starting at coordinates (row,col)  */
bool add_to_scene(char **scene, int row, int col, const char *filename) {

  ifstream in(filename);
  if (!in)
    return false;

  int start_col = col;
  char line[512];
  in.getline(line,512);
  filter(line);
  while (!in.fail()) {
    for (int n=0; n<strlen(line); n++) {
      if (row >= SCENE_HEIGHT)
	return false;
      if (col >= SCENE_WIDTH)
	break;
      scene[row][col++] = line[n];
    }
    row++;
    col = start_col;
    in.getline(line,512);
    filter(line);
  }
  return true;
}

/* pre-supplied helper function to report the status codes encountered in Question 3 */
const char *status_description(int code) {
  switch(code) {
  case ERROR_INVALID_CANNIBAL_COUNT:
    return "Invalid cannibal count";
  case ERROR_INVALID_MISSIONARY_COUNT:
    return "Invalid missionary count";
  case ERROR_INVALID_MOVE:
    return "Invalid move";
  case ERROR_MISSIONARIES_EATEN:
    return "The missionaries have been eaten! Oh dear!";
  case ERROR_INPUT_STREAM_ERROR:
    return "Unexpected input stream error";
  case ERROR_BONUS_NO_SOLUTION:
    return "No solution";
  case VALID_GOAL_STATE:
    return "Problem solved!";
  case VALID_NON_GOAL_STATE:
    return "Valid state, everything is OK!";
  }
  return "Unknown error";
}


/* insert your functions here */

char **make_river_scene(const char* left, const char* boat){
  
  // Create Scene -> output variable
  char **scene = create_scene();
  
  // Add fixed figures
  add_to_scene(scene,0,0,"bank.txt");
  add_to_scene(scene,0,53,"bank.txt");
  add_to_scene(scene,3,30,"sun.txt");
  add_to_scene(scene,19,19,"river.txt");

  // Auxiliar Variables
  bool boat_right = is_boat_at_right(left);
  int left_bank_missionaries = number_of_individuals(left,'M');
  int left_bank_cannibals = number_of_individuals(left,'C');;
  int boat_missionaries = number_of_individuals(boat,'M');
  int boat_cannibals = number_of_individuals(boat,'C');
  int right_bank_cannibals = 3 - left_bank_cannibals - boat_cannibals;
  int right_bank_missionaries = 3 - left_bank_missionaries - boat_missionaries;

  // Add boat figure to scene
  add_to_scene(scene,17,(19+17*boat_right),"boat.txt");

  // Add cannibals and missionaries figures to boat in scene
  int cannibals_left = boat_cannibals;
  int missionaries_left = boat_missionaries;
  for (int i = 0; i<boat_cannibals+boat_missionaries; i++){
    if(missionaries_left>0){
      add_to_scene(scene,11,(22+17*boat_right+6*i),"missionary.txt");
      missionaries_left--;
    }
    else if(cannibals_left>0){
      add_to_scene(scene,11,(22+17*boat_right+6*i),"cannibal.txt");
      cannibals_left--;
    } 
  }
  
  // Add cannibals and missionaries figures to left bank in scene
  for (int i = 0; i<left_bank_cannibals; i++){
    add_to_scene(scene,11,(1+i*6),"cannibal.txt");
  }
  for (int i = 0; i<left_bank_missionaries; i++){
    add_to_scene(scene,2,(1+i*6),"missionary.txt");
  }

  // Add cannibals and missionaries figures to left bank in scene
  for (int i = 0; i<right_bank_cannibals; i++){
    add_to_scene(scene,11,(54+i*6),"cannibal.txt");
  }
  for (int i = 0; i<right_bank_missionaries; i++){
    add_to_scene(scene,2,(54+i*6),"missionary.txt");
  }
  
  return scene;
}

Status perform_crossing(char* left, const char* target){

  // 0) Auxiliar Variables
  bool boat_right;
  int target_missionaries;
  int target_cannibals;
  int left_bank_missionaries;
  int left_bank_cannibals;
  int right_bank_cannibals;
  int right_bank_missionaries;

  boat_right = is_boat_at_right(left);
  target_missionaries = number_of_individuals(target,'M');
  target_cannibals = number_of_individuals(target,'C');
  left_bank_missionaries = number_of_individuals(left,'M');
  left_bank_cannibals = number_of_individuals(left,'C');
  right_bank_missionaries = 3 - left_bank_missionaries;
  right_bank_cannibals = 3 - left_bank_cannibals;
  

  // I) Initial Checks
  if((left_bank_missionaries+right_bank_missionaries)>3)
    return ERROR_INVALID_MISSIONARY_COUNT;
  if((left_bank_cannibals+right_bank_cannibals)>3)
    return ERROR_INVALID_CANNIBAL_COUNT;

  if((target_missionaries+target_cannibals) > 2 || 
    (target_missionaries+target_cannibals) == 0)
    return ERROR_INVALID_MOVE;

  if(boat_right){
    if(right_bank_missionaries<target_missionaries || 
      right_bank_cannibals<target_cannibals)
        return ERROR_INVALID_MOVE; 
  }
  else{
    if(left_bank_missionaries<target_missionaries || 
      left_bank_cannibals<target_cannibals)
        return ERROR_INVALID_MOVE; 
  }

  // II) Loading the Boat
  cout << endl;
  cout << "Loading the boat..." << endl;

  for (int i = 0; i < target_missionaries; i++){
    if(!boat_right)
      remove_char(left,'M');
  }
  for (int i = 0; i < target_cannibals; i++){
    if(!boat_right)
      remove_char(left,'C');
  }
  print_scene(make_river_scene(left,target)); 

  // III) Crossing the River
  cout << endl;
  cout << "Crossing the river..." << endl;
  
  if(!boat_right)
    remove_char(left,'B');
  else
    add_char(left,'B');
  
  print_scene(make_river_scene(left,target));

  // IV) Unloading the boat
  cout << endl;
  cout << "Unloading the boat..." << endl;

  for (int i = 0; i < target_missionaries; i++){
    if(boat_right)
      add_char(left,'M');
  }
  for (int i = 0; i < target_cannibals; i++){
    if(boat_right)
      add_char(left,'C');
  }
  print_scene(make_river_scene(left,""));

  left_bank_missionaries = number_of_individuals(left,'M');
  left_bank_cannibals = number_of_individuals(left,'C');
  right_bank_missionaries = 3 - left_bank_missionaries;
  right_bank_cannibals = 3 - left_bank_cannibals;
  
  // V) Checking outputs

  if((right_bank_cannibals>right_bank_missionaries 
    && right_bank_missionaries !=0) ||
    (left_bank_cannibals>left_bank_missionaries 
    && left_bank_missionaries !=0))
    return ERROR_MISSIONARIES_EATEN;
  if(right_bank_cannibals == 3 && right_bank_missionaries==3)
    return VALID_GOAL_STATE;

  return VALID_NON_GOAL_STATE;
}

Status play_game(){
  char input[3];
  char left[10] = "BMMMCCC";
  
  Status status;
  
  do{
    cout << "Add individuals in boat. Insert 'M' for a missionary and "<< endl;
    cout << "'C' for  a cannibal (max 2 characters): ";
    if(!cin.getline(input, 3)){
      return ERROR_INPUT_STREAM_ERROR;}
    cout << input << endl;
    status = perform_crossing(left,input);
    cout << status_description(status) << endl << endl;
  } while((status != ERROR_MISSIONARIES_EATEN) && (status != VALID_GOAL_STATE));
  return status;
}

// Status find_solution(char* left, char* answer){

//   const char* target_options[]={"M","C","MM","CC","MC"};

//   // Base Case
//   if (strcmp(left,"") == 0)
//     return VALID_GOAL_STATE;

//   // Recursive Case

//   Status result;
//   char temp_1[10];
//   char temp_2[512];

//   for (int i = 0; i < 5; i++){
//     strcpy(temp_1,left);
//     strcpy(temp_2,answer);
//     result=perform_crossing_no_print(left, target_options[i]);
//     if (result == VALID_NON_GOAL_STATE || result == VALID_GOAL_STATE){
//       strcat(answer,target_options[i]);
//       strcat(answer,",");
//       find_solution(left,answer);
//       strcpy(left,temp_1);
//       strcpy(answer,temp_2);
//     }
    
//   }

//   return ERROR_BONUS_NO_SOLUTION;
// }

Status perform_crossing_no_print(char* left, const char* target){

  // 0) Auxiliar Variables
  bool boat_right;
  int target_missionaries;
  int target_cannibals;
  int left_bank_missionaries;
  int left_bank_cannibals;
  int right_bank_cannibals;
  int right_bank_missionaries;

  boat_right = is_boat_at_right(left);
  target_missionaries = number_of_individuals(target,'M');
  target_cannibals = number_of_individuals(target,'C');
  left_bank_missionaries = number_of_individuals(left,'M');
  left_bank_cannibals = number_of_individuals(left,'C');
  right_bank_missionaries = 3 - left_bank_missionaries;
  right_bank_cannibals = 3 - left_bank_cannibals;
  

  // I) Initial Checks
  if((left_bank_missionaries+right_bank_missionaries)>3)
    return ERROR_INVALID_MISSIONARY_COUNT;
  if((left_bank_cannibals+right_bank_cannibals)>3)
    return ERROR_INVALID_CANNIBAL_COUNT;

  if((target_missionaries+target_cannibals) > 2 || 
    (target_missionaries+target_cannibals) == 0)
    return ERROR_INVALID_MOVE;

  if(boat_right){
    if(right_bank_missionaries<target_missionaries || 
      right_bank_cannibals<target_cannibals)
        return ERROR_INVALID_MOVE; 
  }
  else{
    if(left_bank_missionaries<target_missionaries || 
      left_bank_cannibals<target_cannibals)
        return ERROR_INVALID_MOVE; 
  }

  // II) Loading the Boat
  for (int i = 0; i < target_missionaries; i++){
    if(!boat_right)
      remove_char(left,'M');
  }
  for (int i = 0; i < target_cannibals; i++){
    if(!boat_right)
      remove_char(left,'C');
  }

  // III) Crossing the River
  if(!boat_right)
    remove_char(left,'B');
  else
    add_char(left,'B');
  
  // IV) Unloading the boat
  for (int i = 0; i < target_missionaries; i++){
    if(boat_right)
      add_char(left,'M');
  }
  for (int i = 0; i < target_cannibals; i++){
    if(boat_right)
      add_char(left,'C');
  }

  left_bank_missionaries = number_of_individuals(left,'M');
  left_bank_cannibals = number_of_individuals(left,'C');
  right_bank_missionaries = 3 - left_bank_missionaries;
  right_bank_cannibals = 3 - left_bank_cannibals;
  
  // V) Checking outputs

  if((right_bank_cannibals>right_bank_missionaries 
    && right_bank_missionaries !=0) ||
    (left_bank_cannibals>left_bank_missionaries 
    && left_bank_missionaries !=0))
    return ERROR_MISSIONARIES_EATEN;
  if(right_bank_cannibals == 3 && right_bank_missionaries==3)
    return VALID_GOAL_STATE;

  return VALID_NON_GOAL_STATE;
}

int number_of_individuals(const char* input, char individual){
  
  int counter = 0;
  for (int i=0; i < strlen(input); i++){
    
    if (input[i]==individual)
      counter++;
  }
  return counter;
}

bool is_boat_at_right(const char* input){
  
  for (int i=0; i < strlen(input); i++){
    if(input[i]=='B')
      return false;
  }
  return true;
}

void remove_char(char * str, const char chr){
    
  int i = 0;
  int j;
  bool ocurrence = false;
  while(i<strlen(str)){
      if(str[i]==chr && !ocurrence){
          j = i;
          while(j < strlen(str)){
              str[j] = str[j+1];
              j++;
          }
          i--;
          ocurrence = true;
      }
      i++;
  }
}

void add_char(char * str, const char chr){
    
  str[strlen(str)] = chr;
  str[strlen(str)+1] = '\0';
}