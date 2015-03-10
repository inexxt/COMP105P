
struct Sector
{
	int northWall;
	int southWall;
	int westWall;
	int eastWall;
	int visited;
};



extern struct Sector maze[4][4];



//void initialiseMaze();
void updateSector();