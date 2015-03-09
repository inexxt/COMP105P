/*
void initialiseMaze()
{
	int i,j;
	for(i = 0; i < 4; i++)
	{
		for(j = 0; j < 4; j++)
		{
			maze[i][j] = malloc(sizeof(Sector));
		}
	}
}
*/

struct Sector maze[4][4];

struct XY getCurrentSector()
{
// TODO
	
}

void updateSector(int northWall, int southWall, int westWall, int eastWall)
{
	struct XY currentSector = getCurrentSector();
	maze[currentSector.x][currentSector.y].northWall = northWall;
	maze[currentSector.x][currentSector.y].southWall = southWall;
	maze[currentSector.x][currentSector.y].westWall = westWall;
	maze[currentSector.x][currentSector.y].eastWall = eastWall;
	maze[currentSector.x][currentSector.y].visited = 1;
}