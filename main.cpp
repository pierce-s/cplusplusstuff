#include <iostream>
#include <string>
#include <vector>
#include <numeric>
#include<algorithm>
#include<cmath>
#include<Windows.h>
#include<chrono>

using namespace std;

struct Piece;

// defines the grid in terms of rows and columns
struct Grid {
    int rows;
    int cols;
    // determines if the grid represents an external playing field or is just for logic
    bool real = false;
    vector <vector<char> > matrix;

    // initializes matrix
    Grid(int arows = 20, int acols = 10){
        rows = arows;
        cols = acols;

        for (int i = 0; i < rows; i++){
            vector<char> row;

            for (int j = 0; j < cols; j++){
                row.push_back(' ');
            }

        matrix.push_back(row);
        }

    }

    // clears the matrix
    void Clear(){
        for (int e = 0; e < rows; e++){
            for (int f = 0; f < cols; f++){
                matrix[e][f]=(' ');
            }
        }


    }

    // draws a text representation of the grid
    void Draw(){
        cout << "   ";

        for (int i = 0; i <= rows + 1; i++){

            if  ((i != 0) && (i != (rows + 1))) {

                if (i < 11){
                cout << (i-1) << "  ";
                }
                else{
                    cout << (i-1) << " ";
                }
            }

            for (int j = 0; j <= cols; j++){

                if ((i == 0) && (j < cols)){
                    cout << j << " ";
                }
                else if (i == rows + 1){
                    if (j == 0){
                        cout << "   ";
                    }
                    if (j < cols){
                       cout << j << " ";
                    }

                }
                else{
                    if (j == cols){
                        if (i > 0){
                            cout << i-1 << " ";
                        }

                    }
                    else if (matrix[i-1][j] == ' '){
                        cout << "- ";
                    }
                    else {
                        cout << matrix[i-1][j] << " ";
                    }

                }

            }

            cout << endl;
        }
    }
};

// defines each piece
struct Piece{
    // all possible orientations the piece can have
    vector< vector< vector<char> > > shapes;
    // the current orientation of the piece
    vector< vector<char> > orientation;
    // current index of orientation in shapes
    int rotation = 0;
    int height;
    int width;
    char name;

    Piece(char aname = 'X'){
            name = aname;
    }

    // sets current orientation to index rot in shapes
    void Rotate(int rot){
        orientation = shapes[rot];
        rotation = rot;
        height = orientation.size();
        width = orientation[0].size();
    }
    // adds a new orientation to shapes
    void Add_Orientation(vector< vector<char> > orientation){
        shapes.push_back(orientation);
    }

};

// Create Pieces
Piece O('O');
Piece T('T');
Piece I('I');
Piece S('S');
Piece Z('Z');
Piece J('J');
Piece L('L');

Piece X('X');

Piece Find_Piece(Grid& grid, string game,HWND hWND);

// Clear line at height
void Clear_Line(Grid& grid, int line){

    for (int i = line; i >= 0; i-- ){
                // move every row down starting from cleared line
                if (i > 0){
                    for (int c = 0; c < grid.cols; c++){
                    grid.matrix[i][c] = grid.matrix[i-1][c];
                    //grid.matrix[i]=grid.matrix[i-1];
                    }
                }

                // top row becomes empty
                else {
                    grid.matrix[0] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};
                }
            }
}

// Checks if a line needs to be cleared, in a given height range
void Line_Clear_Check(Grid& grid, int start, int stop){
    bool clear_line;
    for (int y = stop; y >= start; y--){
        clear_line = true;
        for (int x = 0; x < grid.cols; x++){
            if (grid.matrix[y][x] == ' '){
                clear_line = false;
                break;
            }
        }
        // if line is full, clear it
        if (clear_line){
            Clear_Line(grid, y);
            // start back at the top if a line is cleared
            y = stop;
            start+=1;
//            if (start < stop){
//                y = start;
//                Line_Clear_Check(grid,start+1,stop);
//            }
        }



    }
}

// if no start/stop paramater given, check every row
void Line_Clear_Check(Grid& grid){
    Line_Clear_Check(grid,0,grid.rows-1);
}

// Hard drop a piece at a given rotation & translation
void Place_Piece(Grid& grid, Piece piece, int rotation, int position){

    piece.Rotate(rotation);

    // check every row
    for (int height = 0; height < grid.rows - (piece.height-1); height++){
        // check piece at every height
        for (int j = piece.height - 1 ;j >= 0 ; j--){
            // check piece at every width
            for (int i = 0; i < piece.width;  i++){
                // if current height + piece at height j is not empty
                if (grid.matrix[j+height][i+position] != ' '){
                    // and if the piece collision point is not empty
                    if (piece.orientation[j][i] != ' '){
                        //cout << "piece hit at" << j << "," << i << endl;
                        //cout << "collision at " << j+height << "," << i+position << endl;
                        // place the piece using the collision point as an offset
                        //cout << "placing " << piece.name << " rotation " << rotation << " position " << position << endl;
                        for (int y = 0; y < piece.height; y++){
                            for (int x = 0; x < piece.width; x++){
                                // fill in blocks
                                if (piece.orientation[y][x] != ' '){

                                        // if collided, check if piece would be placed out of bounds
                                        if ((height+y-1) < 0){
                                            //change later
                                            cout << "cannot place piece" << endl;
                                            goto stop;
                                        }
                                        grid.matrix[height+y-1][position+x]=piece.name;
                                }

                            }
                        }
                        // check if a line will be cleared
                        // only need to check at current height collision - j offset, -1 (piece is placed above collision)
                        // range is from the top of the piece to right above collision point
                        //cout << (height-1+j) - (piece.height-1) << "," << height-1+j << endl;
                        Line_Clear_Check(grid, height -1 , height + piece.height - 2);
                        goto stop;
                    }
                }


            }
        }
    }

    for (int y = 0; y < piece.height; y++){
        for (int x = 0; x < piece.width; x++){
            if (piece.orientation[y][x] != ' '){
                grid.matrix[grid.rows-piece.height + y][position+x] = piece.name;
            }
        }
    }
    Line_Clear_Check(grid, ((grid.rows-1) - (piece.height-1)), (grid.rows-1));
    stop:;


    if (grid.real){
        //grid.Draw();
    }

}




//vector<int> Peaks(Grid grid){
//    vector<int> peaks;
//    for (int width = 0; width < grid.cols; width++){
//        for (int height = 0; height < grid.rows; height++){
//            peaks.push_back(0);
//            if (grid.matrix[height][width] != ' '){
//                peaks[width] = grid.rows - height;
//                break;
//            }
//
//        }
//
//
//    }
//    return peaks;
//}


// -------- Not in use, returns the sum of every column height
//int Height(Grid grid){
//    vector<int> peaks = Peaks(grid);
//    //cout << peaks.begin() << "," << peaks.end() << endl;
//    return accumulate(peaks.begin(), peaks.end(), 0);
//}



int Holes(Grid grid){
    bool Top;
    //int holes = 0;
    double holescore = 0;
    for (int width = 0; width < grid.cols; width++){
        Top = false;
        for (int height = 0; height < grid.rows; height++){
            if (grid.matrix[height][width] != ' '){
                Top = true;
            }
            if (Top){
                if (grid.matrix[height][width] == ' '){
                    //holes +=1;
                    holescore += pow(grid.rows - height , 3);
                }
                if (width > 0){
                    if (grid.matrix[height][width-1] == ' '){
                    //holes +=1;
                    holescore += pow(grid.rows - height , 2);
                    }
                }
                if (width < grid.cols - 1){
                    if (grid.matrix[height][width+1] == ' '){
                    //holes +=1;
                    holescore += pow(grid.rows - height , 2);
                    }

                }
            }

        }

    }
    //cout << "holes: " << holes << endl;
    return (int) holescore;
}

// Sorting algorithm for determining lowest rank
inline bool Sort_Rank(vector<int> a, vector<int> b){
    return a[0] < b[0];
}


HWND g_hWND;

// Takes a given piece and ranks each possible placement
vector< vector<int> >  Rank(Grid grid, Piece piece){
    Grid test;
    Grid test2;
    Grid test3;
    grid.real = false;
    // if set to 1 then one preview piece will be checked
    int previews = 0;
    Piece next_piece;
    Piece current_piece;

    if (previews == 1){
        next_piece = Find_Piece(grid, "Cultris", g_hWND);
    }

    vector< vector<int> > ranks;
    vector<int> ranks2;
    vector<int> ranks3;
    int rank_sum;
//    vector<Piece> pieces{T,O,I,J,S,Z,L}; //
    for (int i = 0; i < piece.shapes.size(); i++){
        piece.Rotate(i);
        for (int j = 0; j <= (grid.cols - piece.width); j++){
            test = grid;
            Place_Piece(test,piece,i,j);


            if (previews == 1){
                for (int x = 0; x < next_piece.shapes.size(); x++){
                    next_piece.Rotate(x);
                    for (int y = 0; y <= (grid.cols - next_piece.width); y++){
                        test2 = test;
                        Place_Piece(test2,next_piece,x,y);


                        // --------Predicts every possible next piece-------------

//                        rank_sum = 0;
//                        for(int p = 0; p < pieces.size(); p++){
//                            current_piece = pieces[p];
//                            ranks3.clear();
//                            for (int xx = 0; xx < current_piece.shapes.size(); xx++){
//                                current_piece.Rotate(xx);
//                                for (int yy = 0; yy <= (grid.cols - current_piece.width); yy++){
//                                    test3 = test2;
//                                    Place_Piece(test3,current_piece,xx,yy);
//                                    ranks3.push_back(Holes(test3));
//                            sort(ranks3.begin(),ranks3.end());
//                            rank_sum += ranks3[0];
//
//                                }
//                            }
//                        }
                        // with rank2
                        ranks2.push_back(Holes(test2));//+rank_sum
                        sort(ranks2.begin(),ranks2.end());




                    }
                }


            }
            // with rank 2
            if (previews == 1) {
                ranks.push_back({Holes(test)+ranks2[0],i,j});
            }
            else {
                ranks.push_back({Holes(test),i,j});
            }

            // without rank 2
//            ranks.push_back({Holes(test),i,j});


        }

    }

    sort(ranks.begin(),ranks.end(), Sort_Rank);

    return ranks;
}


// Executes an individual key press
void Press_Key(SHORT k, int delay = 25, double after_delay = 5){

    SHORT key;
    UINT mappedkey;
    INPUT input = { 0 };
    key = VkKeyScan(k);
    mappedkey = MapVirtualKey(LOBYTE(key), 0);
    input.type = INPUT_KEYBOARD;
    input.ki.dwFlags = KEYEVENTF_SCANCODE;
    input.ki.wScan = mappedkey;
    SendInput(1, &input, sizeof(input));
    Sleep(delay);
    input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(input));
    Sleep(after_delay);
}

// Executes all types of left/right movement combinations for pieces on external game
void Move(char movetype,char movetype2 = 'x', double press_delay = 35, double DAS_delay = 65, double after_delay = 50){
    switch(movetype){
        // tap left once
        case 'l': Press_Key('j', press_delay);
                     break;
        // tap right once
        case 'r': Press_Key('l', press_delay);
                     break;
        // hold left until hitting wall
        case 'L': Press_Key('j', DAS_delay);
                     break;
        // hold right until hitting wall
        case 'R': Press_Key('l', DAS_delay);
                     break;
        default: break;
    }

    Sleep(after_delay);
    if (movetype2 != 'x'){
        Move(movetype2);
    }

}

void Invalid(){
    cout << "Move not found." << endl;
}

// When playing an external tetris game, will place a given piece at desired location with fewest keypresses
void Finesse(Piece piece, int rotation, int position, double press_delay = 35){
    switch(piece.name){
        // Piece Name O
        case 'O':
            switch(rotation){
                // Orientation O 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L','r', press_delay);
                            break;
                        case 2:
                            Move('l','l', press_delay);
                            break;
                        case 3:
                            Move('l', press_delay);
                            break;
                        case 4:
                            break;
                        case 5:
                            Move('r', press_delay);
                            break;
                        case 6:
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Move('R','l', press_delay);
                            break;
                        case 8:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;
                    }
                    break;
            }
            break;
        // Piece Name S
        case 'S':
            switch(rotation){
                // Orientation S 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation S 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 2:
                            Press_Key('a');
                            Move('l', press_delay);
                            break;
                        case 3:
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('d');
                            break;
                        case 5:
                            Press_Key('d');
                            Move('r', press_delay);
                            break;

                            break;
                        case 6:
                            Press_Key('d');
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Press_Key('d');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
        // Piece Name Z
        case 'Z':
            switch(rotation){
                // Orientation Z 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation Z 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 2:
                            Press_Key('a');
                            Move('l', press_delay);
                            break;
                        case 3:
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('d');
                            break;
                        case 5:
                            Press_Key('d');
                            Move('r', press_delay);
                            break;
                        case 6:
                            Press_Key('d');
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Press_Key('d');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
        // Piece Name T
        case 'T':
            switch(rotation){
                // Orientation T 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation T 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('d');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 2:
                            Press_Key('d');
                            Move('l','l', press_delay);
                            break;
                        case 3:
                            Press_Key('d');
                            Move('l', press_delay);
                            break;
                        case 4:
                            Press_Key('d');
                            break;
                        case 5:
                            Press_Key('d');
                            Move('r', press_delay);
                            break;
                        case 6:
                            Press_Key('d');
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Press_Key('d');
                            Move('R','l',press_delay);

                            break;
                        case 8:
                            Move('R', press_delay);
                            Press_Key('d');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation T 2
                case 2:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            Press_Key('s');
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            Press_Key('s');
                            break;
                        case 2:
                            Move('l', press_delay);
                            Press_Key('s');
                            break;
                        case 3:
                            Press_Key('s');
                            break;
                        case 4:
                            Move('r', press_delay);
                            Press_Key('s');
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            Press_Key('s');
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('s');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('s');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation T 3
                case 3:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Press_Key('a');
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Press_Key('a');
                            Move('l', press_delay);
                            break;
                        case 3:
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('a');
                            Move('r', press_delay);
                            break;
                        case 5:
                            Press_Key('a');
                            Move('r','r', press_delay);
                            break;

                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('a');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Press_Key('a');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
        // Piece Name J
        case 'J':
            switch(rotation){
                // Orientation J 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation J 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('d');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 2:
                            Press_Key('d');
                            Move('l','l', press_delay);
                            break;
                        case 3:
                            Press_Key('d');
                            Move('l', press_delay);
                            break;
                        case 4:
                            Press_Key('d');
                            break;
                        case 5:
                            Press_Key('d');
                            Move('r', press_delay);
                            break;
                        case 6:
                            Press_Key('d');
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Press_Key('d');
                            Move('R','l',press_delay);

                            break;
                        case 8:
                            Move('R', press_delay);
                            Press_Key('d');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation J 2
                case 2:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            Press_Key('s');
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            Press_Key('s');
                            break;
                        case 2:
                            Move('l', press_delay);
                            Press_Key('s');
                            break;
                        case 3:
                            Press_Key('s');
                            break;
                        case 4:
                            Move('r', press_delay);
                            Press_Key('s');
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            Press_Key('s');
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('s');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('s');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation J 3
                case 3:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Press_Key('a');
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Press_Key('a');
                            Move('l', press_delay);
                            break;
                        case 3:
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('a');
                            Move('r', press_delay);
                            break;
                        case 5:
                            Press_Key('a');
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('a');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Press_Key('a');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
        // Piece Name L
        case 'L':
            switch(rotation){
                // Orientation L 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            break;
                        case 7:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation L 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('d');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 2:
                            Press_Key('d');
                            Move('l','l', press_delay);
                            break;
                        case 3:
                            Press_Key('d');
                            Move('l', press_delay);
                            break;
                        case 4:
                            Press_Key('d');
                            break;
                        case 5:
                            Press_Key('d');
                            Move('r', press_delay);
                            break;
                        case 6:
                            Press_Key('d');
                            Move('r','r', press_delay);
                            break;
                        case 7:
                            Press_Key('d');
                            Move('R','l',press_delay);

                            break;
                        case 8:
                            Move('R', press_delay);
                            Press_Key('d');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation L 2
                case 2:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            Press_Key('s');
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            Press_Key('s');
                            break;
                        case 2:
                            Move('l', press_delay);
                            Press_Key('s');
                            break;
                        case 3:
                            Press_Key('s');
                            break;
                        case 4:
                            Move('r', press_delay);
                            Press_Key('s');
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            Press_Key('s');
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('s');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('s');
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation L 3
                case 3:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Press_Key('a');
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Press_Key('a');
                            Move('l', press_delay);
                            break;
                        case 3:
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('a');
                            Move('r', press_delay);
                            break;
                        case 5:
                            Press_Key('a');
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R','l', press_delay);
                            Press_Key('a');
                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Press_Key('a');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
        // Piece Name I
        case 'I':
            switch(rotation){
                // Orientation I 0
                case 0:
                    switch(position){
                        // Position
                        case 0:
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('l','l', press_delay);
                            break;
                        case 2:
                            Move('l', press_delay);
                            break;
                        case 3:
                            break;
                        case 4:
                            Move('r', press_delay);
                            break;
                        case 5:
                            Move('r','r', press_delay);
                            break;
                        case 6:
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;
                // Orientation I 1
                case 1:
                    switch(position){
                        // Position
                        case 0:
                            Press_Key('a');
                            Move('L', press_delay);
                            break;
                        case 1:
                            Move('L', press_delay);
                            Press_Key('a');
                            break;
                        case 2:
                            Move('L', press_delay);
                            Press_Key('d');
                            break;
                        case 3:
                            Move('l', press_delay);
                            Press_Key('a');
                            break;
                        case 4:
                            Press_Key('a');
                            break;
                        case 5:
                            Press_Key('d');
                            break;
                        case 6:
                            Move('r', press_delay);
                            Press_Key('d');
                            break;

                            break;
                        case 7:
                            Move('R', press_delay);
                            Press_Key('a');
                            break;
                        case 8:
                            Move('R', press_delay);
                            Press_Key('d');
                            break;
                        case 9:
                            Press_Key('d');
                            Move('R', press_delay);
                            break;
                        default:
                            Invalid();
                            break;

                    }
                    break;


            }
            break;
    }

    Press_Key(VK_SPACE, press_delay);
}

// Automatically Places a piece based on the best ranked rotation & translation
void Auto_Place(Grid& grid, Piece piece, string game = "None"){
    int rotation = Rank(grid,piece)[0][1];
    int position = Rank(grid,piece)[0][2];

    // Places piece on internal matrix ( for logic)
    Place_Piece(grid, piece, rotation, position) ;
    // Outputs key presses to place the piece on external game
    if ((game != "None") && grid.real){
        Finesse(piece, rotation, position);
    }
}



// Places 500 Pieces and times it
void Speed_Test(Grid grid){
    int bags = 5;
    int reps = 10;
    grid.real = false;
    auto start = std::chrono::high_resolution_clock::now();

    for (int j = 0; j <= reps ; j++){
        for (int i = 0; i <= bags ; i++){
            Auto_Place(grid,T);
            Auto_Place(grid,S);
            Auto_Place(grid,Z);
            Auto_Place(grid,I);
            Auto_Place(grid,O);
            Auto_Place(grid,J);
            Auto_Place(grid,L);
            Auto_Place(grid,T);
            Auto_Place(grid,S);
            Auto_Place(grid,Z);
        }
        grid.Clear();
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = finish - start;

    double pieces = ((bags*10)*reps);
    cout << pieces << " pieces placed." << endl;
    cout << "Elapsed time: " << elapsed.count() << " s\n";
    cout << pieces / elapsed.count() << " pieces per second" << endl;
}

static void Init_Pieces(){

// Add orientations for each piece

//    Piece O('O');
    O.Add_Orientation({{'O','O'},
                       {'O','O'}});

//    Piece T('T');
    T.Add_Orientation({{' ','T',' '},
                       {'T','T','T'}});

    T.Add_Orientation({{'T',' '},
                       {'T','T'},
                       {'T',' '}});

    T.Add_Orientation({{'T','T','T'},
                       {' ','T',' '}});

    T.Add_Orientation({{' ','T'},
                       {'T','T'},
                       {' ','T'}});

//    Piece I('I');
    I.Add_Orientation({{'I',
                        'I',
                        'I',
                        'I'}});

    I.Add_Orientation({{'I'},{'I'},{'I'},{'I'}});

//    Piece S('S');
    S.Add_Orientation({{' ','S','S'},
                       {'S','S',' '}});

    S.Add_Orientation({{'S',' '},
                       {'S','S'},
                       {' ','S'}});

//    Piece Z('Z');
    Z.Add_Orientation({{'Z','Z',' '},
                       {' ','Z','Z'}});

    Z.Add_Orientation({{' ','Z'},
                       {'Z','Z'},
                       {'Z',' '}});

//    Piece J('J');
    J.Add_Orientation({{'J',' ',' '},
                       {'J','J','J'}});

    J.Add_Orientation({{'J','J'},
                       {'J',' '},
                       {'J',' '}});

    J.Add_Orientation({{'J','J','J'},
                       {' ',' ','J'}});

    J.Add_Orientation({{' ','J'},
                       {' ','J'},
                       {'J','J'}});

//    Piece L('L');
    L.Add_Orientation({{' ',' ','L'},
                       {'L','L','L'}});

    L.Add_Orientation({{'L',' '},
                       {'L',' '},
                       {'L','L'}});

    L.Add_Orientation({{'L','L','L'},
                       {'L',' ',' '}});

    L.Add_Orientation({{'L','L'},
                       {' ','L'},
                       {' ','L'}});
}



Piece Find_Piece(Grid& grid, string game,HWND hWND){

    int xpos;
    int ypos;
    Piece found('F');
    if (game == "NullpoMino"){

        xpos = 443;
        ypos = 183;
    }
    else if (game == "Cultris"){
        xpos = 420;
        ypos = 188;
    }


    else {
        cout << "No game selected, cannot search for piece" << endl;

    }

    HDC hDC = GetDC(hWND);
    POINT p;
    ScreenToClient(hWND, &p);
    p.x = xpos;
    p.y = ypos;
    COLORREF color = GetPixel(hDC, xpos, ypos);
    ReleaseDC(hWND,hDC);

    if (game == "NullpoMino"){
        switch(color){
            case 14760482: return J;
                           break;
            case 14754465: return T;
                           break;
            case 2281953:  return O;
                           break;
            case 14797090: return I;
                           break;
            case 2285858:  return S;
                           break;
            case 3810017:  return Z;
                           break;
            case 2261473:  return L;
                           break;
            default: cout << "Not found" << endl;
                     //return 0;
                     break;
            }
    }
    else if (game == "Cultris"){
        switch(color){
            case 7744042: return J;
                           break;
            case 7740022: return T;
                           break;
            case 2782833:  return O;
                           break;
            case 7763482: return I;
                           break;
            case 1078800:  return S;
                           break;
            case 1710710:  return Z;
                           break;
            case 1721196:  return L;
                           break;
            default: cout << color << " not found." << endl;

                     //return 0;
                     break;
            }
    }

}



void Auto_Play(Grid& grid, string game){

   // while (true){


        Piece piece('F');
        grid.Clear();
        Piece next_piece('F');


        // Make Window Active
        LPCWSTR window_title;
        HWND hWND;
        if (game == "NullpoMino"){
            window_title = L"NullpoMino - LINE RACE";
            hWND = FindWindowW(NULL, window_title);
            SwitchToThisWindow(hWND,false);
            Sleep(50);
            Press_Key(VK_RETURN);
            Sleep(250);
            piece = Find_Piece(grid, game, hWND);
            Sleep(1550);
        }
        else if(game == "Cultris"){
            window_title = L"Cultris II";
            hWND = FindWindowW(NULL, window_title);
            SwitchToThisWindow(hWND,false);
            Sleep(50);

            g_hWND = hWND;
            cout << "Waiting for piece";
            while (true){
                Sleep(250);
                cout << ".";
                if (GetAsyncKeyState(0x4A)){
                    piece = J;
                    break;
                }
                else if (GetAsyncKeyState(0x4C)){
                    piece = L;
                    break;
                }
                else if (GetAsyncKeyState(0x53)){
                    piece = S;
                    break;
                }
                else if (GetAsyncKeyState(0x5A)){
                    piece = Z;
                    break;
                }
                else if (GetAsyncKeyState(0x4F)){
                    piece = O;
                    break;
                }
                else if (GetAsyncKeyState(0x54)){
                    piece = T;
                    break;
                }
                else if (GetAsyncKeyState(0x49)){
                    piece = I;
                    break;
                }
            }
        }
        else{

            return;
        }


        next_piece = Find_Piece(grid, game, hWND);

        while (true) {

            if (GetAsyncKeyState(VK_NUMPAD0)) { // Exit
                break;
            }

            Auto_Place(grid, piece, game);
            piece = next_piece;
            next_piece = Find_Piece(grid, game, hWND);

        }
        cout << "terminated" << endl;
        grid.Draw();

}



int main(){
    int rows = 20;
    int cols = 10;
    Grid grid(rows,cols);
    grid.real = true;
    string game_name;

    // Select game
    //game_name = "NullpoMino";
    game_name = "Cultris";
    //game_name = "test";
    Init_Pieces();

    // Start tetris ai
    Auto_Play(grid, game_name);


    //Speed_Test(grid);




    return 0;
}



