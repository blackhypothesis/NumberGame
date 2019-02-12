#define OLC_PGE_APPLICATION
#include <array>
#include <algorithm>
#include <random>
#include <thread>
#include "olcPixelGameEngine.h"

using namespace std;

typedef struct sPoint
{
	int x;
	int y;
} point;



vector<int> shuffleNumbers(int nSize)
{
	vector<int> vecRandNumbers;
	for (int i = -nSize / 2; i <= nSize / 2; i++)
	{
		// omit 0 if nsize is even
		if (nSize % 2 == 0) {
			if (!i == 0)
				vecRandNumbers.push_back(i);
		}
		else
		{
			vecRandNumbers.push_back(i);
		}
		
	}
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	auto rng = default_random_engine{seed};
	shuffle(begin(vecRandNumbers), end(vecRandNumbers), rng);
 	return  vecRandNumbers;
}

class cBoxWithNumber
{
public:
	cBoxWithNumber(point p, int n)
	{
		this->px = p.x;
		this->py = p.y;
		this->number = n;
	}

	~cBoxWithNumber() {}

	int px;
	int py;

private:
	int number;
	bool isSelected = false;
	bool isHighlighted = false;
	bool isUsable = false;
	olc::Pixel colorFrame;
	olc::Pixel colorBg;
	olc::Pixel colorFrameNotSelected = olc::BLACK;
	olc::Pixel colorBgNotSelected = olc::WHITE;
	olc::Pixel colorFrameSelected = olc::RED;
	olc::Pixel colorBgSelected = olc::Pixel(64, 64, 64);
	olc::Pixel colorBgHighlighted = olc::YELLOW;
	olc::Pixel colorBgUsable = olc::GREEN;
	olc::Pixel numberColor;
	static const int offset = 15;
	static const int size = 80;
	static const int gap = 6;
	//static const int offset = 5;
	//static const int size = 46;
	//static const int gap = 3;

public:
	void setNumber(int n) {	number = n; }
	int getNumber() { return number; }

	void setSelected(bool b) { isSelected = b; }
	bool getSelected() { return isSelected; }

	void setHighlighted(bool b)	{ isHighlighted = b; }

	void setUsable(bool b) { isUsable = b; }
	bool getUsable() { return isUsable;	}

public:
	void Draw(olc::PixelGameEngine *engine)
	{
		string s;
		int x1 = offset + px * (size + gap);
		int y1 = offset + py * (size + gap);
		isUsable ? colorBg = colorBgUsable : colorBg = colorBgNotSelected;
		isHighlighted ? colorBg = colorBgHighlighted : true;
		isSelected ? colorFrame = colorFrameSelected : colorFrame = colorFrameNotSelected;
		isSelected ? colorBg = colorBgSelected : true;
		number < 0 ? numberColor = olc::RED : numberColor = olc::BLUE;
		number < 0 ? s = to_string(-number) : s = to_string(number);
		int sl = s.length();
		engine->DrawRect(x1, y1, size, size, colorFrame);
		engine->FillRect(x1 + 1, y1 + 1, size - 2, size - 2, colorBg);
		//engine->DrawString(x1 + (size - 30) / 2 + (4 - sl) * 4, y1 + 12 + (size - 30) / 2, s, numberColor);
		engine->DrawString(x1 + (size - 120) / 2 + (4 - sl) * 16, y1 + 0 + (size - 30) / 2, s, numberColor, 4);
	}

public:
	static point getBox(int x, int y)
	{
		int xb = (x - offset) / (size + gap);
		int yb = (y - offset) / (size + gap);
		return point{ xb, yb };
	}

	static point getBox(int x, int y, int nRow, int bHorizontal)
	{
		int xb = (x - offset) / (size + gap);
		int yb = (y - offset) / (size + gap);
		if (!bHorizontal)
			return point{ xb, nRow };
		else
			return point{ nRow, yb };
	}
};


class cUsable
{
public:
	cUsable(int nFieldSize)
	{
		for (int i = 0; i < nFieldSize * nFieldSize; i++)
		{
			vecUsable.push_back(false);
		}
		this->nFieldSize = nFieldSize;
		setRow();
		bHorizontal = false;
	}

private:
	vector<bool> vecUsable;
	int nFieldSize;
	int nHVRow;
	bool bHorizontal;

public:
	int getRow() { return nHVRow; }
	void setRow() { nHVRow = rand() % nFieldSize; }

public:
	void setAllBoxUsable(vector<cBoxWithNumber*> vecBox)
	{
		for (auto &p : vecBox) { p->setUsable(true); }
	}

	void setBoxUsable(int gt, vector<cBoxWithNumber*> vecBox, point highlightedBox)
	{
		// reset all boxes to not usable
		for (auto &p : vecBox) { p->setUsable(false); }

		switch (gt)
		{
		case 0:
		{
			for (auto &p : vecBox)
			{
				if (bHorizontal)
				{
					if (p->px == highlightedBox.x)
						p->setUsable(true);
				}
				else
				{
					if (p->py == highlightedBox.y)
						p->setUsable(true);
				}
			}
			bHorizontal = !bHorizontal;
		} break;

		case 1:
		{
			for (auto &p : vecBox)
			{
				if (p->px == highlightedBox.x && (p->py + 1) == highlightedBox.y)
					p->setUsable(true);
				if (p->px == highlightedBox.x && (p->py - 1) == highlightedBox.y)
					p->setUsable(true);
				if ((p->px + 1) == highlightedBox.x && p->py == highlightedBox.y)
					p->setUsable(true);
				if ((p->px + 1) == highlightedBox.x && (p->py - 1) == highlightedBox.y)
					p->setUsable(true);
				if ((p->px + 1) == highlightedBox.x && (p->py + 1) == highlightedBox.y)
					p->setUsable(true);
				if ((p->px - 1) == highlightedBox.x && p->py == highlightedBox.y)
					p->setUsable(true);
				if ((p->px - 1) == highlightedBox.x && (p->py - 1) == highlightedBox.y)
					p->setUsable(true);
				if ((p->px - 1) == highlightedBox.x && (p->py + 1) == highlightedBox.y)
					p->setUsable(true);
			}
		} break;
		}
	}
};



// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";		
	}

private:
	array<string, 2> aTypeOfGame = { "alternate bar", "neighbour" };
	vector<cBoxWithNumber*> vecBox;
	int vecPos = 0;
	int nFieldSize = 5;
	int nNewFieldSize = nFieldSize;
	vector<int> randNumbers;
	int nTotal1 = 0;
	int nTotal2 = 0;
	bool bUser1 = true;
	int typeOfGame = 0;
	cUsable usable = cUsable(nFieldSize);
	point newHighlightedBox;
	point oldHighlightedBox;
	point lastSelectedBox = { -1, -1 };


public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		randNumbers = shuffleNumbers(nFieldSize * nFieldSize);

		for (int x = 0; x < nFieldSize; x++)
		{
			for (int y = 0; y < nFieldSize; y++)
			{
				point p = { x, y };
				vecBox.push_back(new cBoxWithNumber(p, randNumbers.at(x + y * nFieldSize)));
			}
		}
		usable.setAllBoxUsable(vecBox);
		drawBoard();

		return true;
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame

		int mouseX = GetMouseX();
		int mouseY = GetMouseY();

		newHighlightedBox = cBoxWithNumber::getBox(mouseX, mouseY);

		// Highlight the Box to which the mouse is pointing to
		for (auto &p : vecBox)	
		{
			p->setHighlighted(false);
			if (p->px == newHighlightedBox.x && p->py == newHighlightedBox.y)
				p->setHighlighted(true);
		}
		if ((newHighlightedBox.x != oldHighlightedBox.x) || (newHighlightedBox.y != oldHighlightedBox.y))
		{
			oldHighlightedBox = newHighlightedBox;
			drawBoard();
		}

		// check if box is usable and associate number to the user
		if (GetMouse(0).bPressed)
			for (auto &p : vecBox)
				if (p->px == newHighlightedBox.x && p->py == newHighlightedBox.y)
				{
					// not already selected and usable
					if (!p->getSelected() && p->getUsable())
					{
						lastSelectedBox = newHighlightedBox;
						p->setSelected(!p->getSelected());

						if (p->getSelected())
							if (bUser1) {
								nTotal1 += p->getNumber();
							}
							else
							{
								nTotal2 += p->getNumber();
							}

						bUser1 = !bUser1;

						// set boxes usable for the next move according to type of game
						usable.setBoxUsable(typeOfGame, vecBox, lastSelectedBox);

						drawBoard();
					}
				}

		// new game
		if (GetKey(olc::Key::R).bReleased)
		{
			newGame();
			drawBoard();
		}

		// increase the size of the board
		if (GetKey(olc::Key::S).bReleased)
		{
			nNewFieldSize++;
			nNewFieldSize > 10 ? nNewFieldSize = 10 : true;
			newGame();
			drawBoard();
		}

		// decrease the size of the board
		if (GetKey(olc::Key::A).bReleased)
		{
			nNewFieldSize--;
			nNewFieldSize < 1 ? nNewFieldSize = 1 : true;
			newGame();
			drawBoard();
		}

		// set type of game
		if (GetKey(olc::Key::T).bReleased)
		{
			typeOfGame++;
			if (typeOfGame > (aTypeOfGame.size() - 1)) {
				typeOfGame = 0;
			}
			if (lastSelectedBox.x != -1 && lastSelectedBox.y != -1)
			{
				usable.setBoxUsable(typeOfGame, vecBox, lastSelectedBox);
				drawBoard();
			}
		}

		return true;
	}

private:
	void newGame()
	{
		// first delete old board (free resources)
		for (auto &p : vecBox)
		{
			delete(p);
		}
		vecBox.clear();

		// then create new board
		nFieldSize = nNewFieldSize;
		randNumbers = shuffleNumbers(nFieldSize * nFieldSize);

		for (int x = 0; x < nFieldSize; x++)
		{
			for (int y = 0; y < nFieldSize; y++)
			{
				point p = { x,y };
				vecBox.push_back(new cBoxWithNumber(p, randNumbers.at(x + y * nFieldSize)));
			}
		}

		int i = 0;
		for (auto & p : vecBox)
		{
			p->setNumber(randNumbers.at(i));
			p->setHighlighted(false);
			p->setSelected(false);
			nTotal1 = 0;
			nTotal2 = 0;
			bUser1 = true;
			i++;
		}

		usable.setAllBoxUsable(vecBox);
	}

private:
	void drawBoard()
	{
		int dx = 960;
		int dy = 20;
		string s1 = to_string(nTotal1);
		string s2 = to_string(-nTotal2);
		string key_r = "[R] Restart Game";
		string key_t = "[T] Type of Game";
		string key_a = "[A] Decrease Board";
		string key_s = "[S] Increase Board";

		FillRect(0, 0, 1299, 899, olc::Pixel(200, 200, 255));
		// draw board
		for (auto &p : vecBox)	p->Draw(this);

		DrawRect(dx, dy, 320, 600, olc::BLACK);
		// draw gathered points
		DrawString(dx + 80, dy + 120, s1, olc::BLUE, 4);
		DrawString(dx + 200, dy + 120, s2, olc::RED, 4);

		DrawString(dx + 20, dy + 260, "Type of Game", olc::BLACK, 2);
		DrawString(dx + 20, dy + 300, aTypeOfGame[typeOfGame], olc::BLACK, 2);

		DrawString(dx + 20, dy + 400, key_r, olc::BLACK, 2);
		DrawString(dx + 20, dy + 440, key_t, olc::BLACK, 2);
		DrawString(dx + 20, dy + 480, key_a, olc::BLACK, 2);
		DrawString(dx + 20, dy + 520, key_s, olc::BLACK, 2);

		FillCircle(dx + 160, dy + 40, 30, bUser1 ? olc::BLUE : olc::RED);
	}

};

int main()
{
	Example numberGame;
	if (numberGame.Construct(1300, 900, 1, 1))
		numberGame.Start();
	return 0;
}