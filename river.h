const int SCENE_HEIGHT=24, SCENE_WIDTH=72;

enum Status {
    VALID_GOAL_STATE=2, 
    VALID_NON_GOAL_STATE=1,
    ERROR_INVALID_CANNIBAL_COUNT=-1, 
    ERROR_INVALID_MISSIONARY_COUNT=-2, 
    ERROR_INVALID_MOVE=-3, 
    ERROR_MISSIONARIES_EATEN=-4, 
    ERROR_INPUT_STREAM_ERROR=-5, 
    ERROR_BONUS_NO_SOLUTION=-6
};

char **allocate_2D_array(int rows, int columns); // internal use
void deallocate_2D_array(char **m, int rows); // internal use
void filter(char *line); // internal use

char **create_scene();
void destroy_scene(char **scene);
void print_scene(char **scene);
bool add_to_scene(char **scene, int row, int col, const char *filename);
const char *status_description(int code);

char **make_river_scene(const char* left, const char* boat);
Status perform_crossing(char* left, const char* target);
Status play_game();
Status find_solution(char* left, char* answer);


int number_of_individuals(const char* input, char individuals);
bool is_boat_at_right(const char* input);
void remove_char(char* string, char letter);
void add_char(char* string, char letter);
bool is_crossing_valid(char* left, const char* target);
bool find_solution_util(char* left, char* answer);
Status perform_crossing_no_print(char* left, const char* target);

