#include <conio.h>
#include "Initializer.h"
#include "resources.h"
#include "MouseInput.h"
#include "Utils.h"
#include "ButtonUtils/Button.h"
#include "SaveFileManager.h"

#define BigInt unsigned long long
#define QUEST_TEXT_COLOR RGB(141,110,99)
#define DEFAULT_MONEY 100000
#define DEFAULT_MPS 10

HANDLE CONSOLE_INPUT, CONSOLE_OUTPUT;
HWND WINDOW_HANDLE;
ImageLayer layer;

void initLayer();
void gotoXY(int, int);
Button createButton(int, int, char*, char*, char*, int, void(*onClick)(Button*));
Button createCollider(int, int, char*, void(*onHover)(Button*));
void printText(HDC, int, int, int, int, COLORREF, int, char*);
void textPositionTester(int, int, COLORREF, int, char*);
void beginStartScreen();
void getUserName();
void beginMapScreen();
void beginStoryScreen();

char lastName[100], firstName[100];
BigInt money, userExp, mps;
int level;
int isFirstOfficeEnabled, isMyBuildingEnabled, isCasinoEnabled;

int main() {
	initialize();
	initLayer();
	Sleep(300);

	beginStartScreen();
	Sleep(500);

	if (!isFileExist(DIR_NAME)) {
		getUserName();
		Sleep(500);
	}
	loadName(lastName, firstName);

	if (!isFileExist(DIR_COMPANY_NAME)) {
		beginStoryScreen();
		Sleep(1000);
	}

	beginMapScreen();
}

void onButtonClick(Button* clickedButton) {
	if (clickedButton->normal == FILE_START_BUTTON) {
		layer.fadeOut(&layer, NULL);
		stopButtonListener();
	}
}

void beginStartScreen() {
	const Button startButton = createButton(1114, 1150, FILE_START_BUTTON, FILE_START_BUTTON_HOVER, FILE_START_BUTTON_CLICK, 2, onButtonClick);
	Button buttons[1] = { startButton };

	Image titleImages[3] = {
		{FILE_TITLE, 0, 0},
		{FILE_TITLE_TEXT, 638, 134},
		{startButton.normal, startButton.x, startButton.y}
	};
	layer.images = titleImages;
	layer.imageCount = 3;
	layer.renderAll(&layer);

	startButtonListener(buttons, 1, &layer);
}

void initUserNameScreen(const int x) {
	layer.renderAll(&layer);
	printText(layer._consoleDC, 1440, 300, 80, 0, RGB(0, 0, 0), TA_CENTER, "성, 이름을 입력해주세요");
	printText(layer._consoleDC, x, 700, 60, 0, RGB(0, 0, 0), TA_LEFT, "  성 :");
	printText(layer._consoleDC, x, 800, 60, 0, RGB(0, 0, 0), TA_LEFT, "이름 :");
}

void getUserName() {
	const int x = 1250;

	Image images[2] = { { FILE_COIN_BLUR, 0, 0 } };
	layer.images = images;
	layer.imageCount = 1;
	initUserNameScreen(x);

	char* editingText = lastName;
	int i = 0;
	while (1) {
		const int input = _getch();

		if (input == '\r') {
			if (editingText == lastName) {
				i = 0;
				editingText = firstName;
				continue;
			}
			break;
		}
		if (input == '\b') {
			if (editingText[i - 1] < -1) i -= 3;
			else i -= 2;
			if (i < -1) i = -1;
			initUserNameScreen(x);
		}
		else if (i >= 6 || input == ' ' || input == '\\') continue;
		else {
			editingText[i] = input;
			if (input >= 128) {
				editingText[++i] = _getch();
			}
		}
		editingText[i + 1] = '\0';
		printText(layer._consoleDC, x + 210, 700, 60, 0, RGB(0, 0, 0), TA_LEFT, lastName);
		printText(layer._consoleDC, x + 210, 800, 60, 0, RGB(0, 0, 0), TA_LEFT, firstName);

		i++;
	}
	layer.images[0].fileName = "";
	layer.renderAll(&layer);
	saveName(trim(lastName), trim(firstName));
}

void printStoryStartText(HDC hdc) {
	char name[100];
	sprintf(name, "23살 1인 앱 개발자 %s%s...", lastName, firstName);
	printText(hdc, 1440, 750, 60, 0, RGB(255, 255, 255), TA_CENTER, name);
}

void printStory1Text(HDC hdc) {
	printText(hdc, 100, 100, 60, 0, RGB(0, 0, 0), TA_LEFT, "밥 먹고 코딩하는 게 일상이었던 나...");
	printText(hdc, 100, 200, 60, 0, RGB(0, 0, 0), TA_LEFT, "여느 때와 다름없이 오늘도 앱을 개발하고 스토어에 올렸다.");
}

void printStoryAfterText(HDC hdc) {
	printText(hdc, 1440, 750, 60, 0, RGB(255, 255, 255), TA_CENTER, TEXT("한달 후..."));
}

void printStory2Text(HDC hdc) {
	const int x = 2800, y = 1250;
	printText(hdc, x, y, 60, 0, RGB(0, 0, 0), TA_RIGHT, TEXT("이게 뭐야!!!!"));
	printText(hdc, x, y + 70, 60, 0, RGB(0, 0, 0), TA_RIGHT, TEXT("평소에는 며칠이 지나도 오르지 않던 다운로드 수 그래프가"));
	printText(hdc, x, y + 140, 60, 0, RGB(0, 0, 0), TA_RIGHT, TEXT("가파른 상승곡선을 이루고 있었다!!"));
}

void printStory3Text(HDC hdc) {
	printText(hdc, 1000, 250, 60, 0, RGB(0, 0, 0), TA_LEFT, TEXT("좋았어 이제 회사를 차려보자!!"));
}

void beginStoryScreen() {
	Image images[1] = { {"", 0, 0} };
	layer.imageCount = 1;
	layer.images = images;

	layer.images[0].fileName = "";
	layer.fadeIn(&layer, printStoryStartText);
	Sleep(2000);
	layer.fadeOut(&layer, printStoryStartText);

	layer.images[0].fileName = FILE_STORY_1;
	layer.fadeIn(&layer, printStory1Text);
	Sleep(2000);
	layer.fadeOut(&layer, printStory1Text);

	layer.images[0].fileName = "";
	layer.fadeIn(&layer, printStoryAfterText);
	Sleep(500);
	layer.fadeOut(&layer, printStoryAfterText);

	layer.images[0].fileName = FILE_STORY_2;
	layer.fadeIn(&layer, printStory2Text);
	Sleep(3000);
	layer.fadeOut(&layer, printStory2Text);

	layer.images[0].fileName = FILE_STORY_3;
	layer.fadeIn(&layer, printStory3Text);
	Sleep(2000);
	layer.images[0].fileName = "";
	layer.renderAll(&layer);
}

#define MAP_IMAGE_COUNT 10
#define MAP_BUTTON_COUNT 7
#define MAP_INDEX 0
#define FIRST_OFFICE_INDEX 1
#define MY_BUILDING_INDEX 2
#define ESTATE_INDEX 3
#define CASINO_INDEX 4
#define LEVEL_BACKGROUND_INDEX 5
#define LEVEL_PROGRESS_INDEX 6
#define MONEY_BACKGROUND_INDEX 7
#define BUILDING_BUTTON_INDEX_OFFSET (-1)
#define EXP_DETAIL_COLLIDER_INDEX 4
#define QUEST_BUTTON_INDEX_IN_LAYER 8
#define QUEST_BUTTON_INDEX_IN_BUTTON 5
#define SETTING_BUTTON_INDEX_IN_LAYER 9
#define SETTING_BUTTON_INDEX_IN_BUTTON 6

void initGetCompanyNameScreen() {
	layer.images[5] = (Image){ FILE_QUEST_WINDOW_NO_TITLE, 730, 380 };
	layer.imageCount = 6;
	layer.renderAll(&layer);

	printText(layer._consoleDC, 1440, 500, 70, 0, QUEST_TEXT_COLOR, TA_CENTER, "회사 이름을 입력해주세요");
}

void getCompanyNameIfNotExist() {
	if (isFileExist(DIR_COMPANY_NAME)) return;
	initGetCompanyNameScreen();

	char companyName[100];
	int i = 0;
	while (1) {
		const int input = _getch();

		if (input == '\r') {
			break;
		}
		if (input == '\b') {
			if (companyName[i - 1] < -1) i -= 3;
			else i -= 2;
			if (i < -1) i = -1;
		}
		else if (i >= 30 || input == ' ' || input == '\\') continue;
		else {
			companyName[i] = input;
			if (input >= 128) {
				companyName[++i] = _getch();
			}
		}
		companyName[i + 1] = '\0';
		initGetCompanyNameScreen();
		printText(layer._consoleDC, 1440, 830, 70, 0, QUEST_TEXT_COLOR, TA_CENTER, companyName);

		i++;
	}

	saveCompanyName(companyName);
}

void updateUserValues() {
	loadMoneyAndExp(&money, &userExp);
	level = (int)sqrtl((long double)(userExp / 400));

	loadMPS(&mps);
}

BigInt getExpForLevel(int level) {
	return (BigInt)(400 * (pow(level, 2)));
}

BigInt getTotalExpForLevel(int level) {
	return getExpForLevel(level + 1) - getExpForLevel(level);
}

BigInt getAchievedExp() {
	return userExp - getExpForLevel(level);
}

int getProgressFromExp() {
	const BigInt totalExp = getTotalExpForLevel(level);
	const BigInt achievedExp = getAchievedExp();
	return (int)((long double)achievedExp / totalExp * 10);
}

void displayExpDetail(HDC hdc) {
	char expText[100];
	sprintf(expText, "%lld/%lld", getAchievedExp(), getTotalExpForLevel(level));
	printText(hdc, 540, 100, 50, 10, RGB(255, 255, 255), TA_LEFT, expText);
}

int isExpDetailShow = 0;
void applyUserValuesToDC(HDC hdc) {
	static char LEVEL_PROGRESS_FILE_NAME[100];
	sprintf(LEVEL_PROGRESS_FILE_NAME, FILE_LEVEL_PROGRESS, getProgressFromExp());
	layer.images[LEVEL_PROGRESS_INDEX].fileName = LEVEL_PROGRESS_FILE_NAME;

	char levelString[10];
	sprintf(levelString, "Lv.%d", level);
	printText(hdc, 242, 125, 100, 0, RGB(0, 0, 0), TA_CENTER, levelString);

	char moneyString[100];
	sprintf(moneyString, "%lld", money);
	printText(hdc, 1390, 90, 70, 0, RGB(255, 255, 255), TA_LEFT, moneyString);
	printText(hdc, 2080, 100, 50, 0, RGB(255, 255, 255), TA_RIGHT, "원");

	char mpsString[100];
	sprintf(mpsString, "%lld원/초", mps);
	printText(hdc, 2080, 200, 40, 300, RGB(255, 255, 255), TA_RIGHT, mpsString);

	if (isExpDetailShow) {
		displayExpDetail(hdc);
		isExpDetailShow = 0;
	}
}

Button buttons[MAP_BUTTON_COUNT];

void onButtonInMapHovered(Button* hoveredButton) {
	if (hoveredButton->normal == FILE_LEVEL_PROGRESS_DEFAULT) isExpDetailShow = 1;
}

void onButtonInMapClicked(Button* clickedButton) {
}

void initMapUI() {
	if (!isFileExist(DIR_MONEY_AND_EXP))
		saveMoneyAndExp(DEFAULT_MONEY, 0);
	if (!isFileExist(DIR_MPS))
		saveMPS(DEFAULT_MPS);
	updateUserValues();

	char LEVEL_PROGRESS_FILE_NAME[100];
	sprintf(LEVEL_PROGRESS_FILE_NAME, FILE_LEVEL_PROGRESS, getProgressFromExp());

	const Button questButton = createButton(100, 1200, FILE_QUEST_BUTTON, FILE_QUEST_BUTTON_HOVER, FILE_QUEST_BUTTON_CLICK, QUEST_BUTTON_INDEX_IN_LAYER, onButtonInMapClicked);
	const Button settingButton = createButton(2600, 65, FILE_SETTING_BUTTON, FILE_SETTING_BUTTON_HOVER, FILE_SETTING_BUTTON_CLICK, SETTING_BUTTON_INDEX_IN_LAYER, onButtonInMapClicked);
	buttons[QUEST_BUTTON_INDEX_IN_BUTTON] = questButton;
	buttons[SETTING_BUTTON_INDEX_IN_BUTTON] = settingButton;

	const Image levelBackground = { FILE_LEVEL_BACKGROUND, 65, 65 };
	const Image levelProgress = { LEVEL_PROGRESS_FILE_NAME, 455,65 };
	const Image moneyBackground = { FILE_MONEY_BACKGROUND, 1250, 65 };
	const Image questButtonImage = { questButton.normal, 100, 1200 };
	const Image settingButtonImage = { settingButton.normal, 2600, 65 };

	layer.images[LEVEL_BACKGROUND_INDEX] = levelBackground;
	layer.images[LEVEL_PROGRESS_INDEX] = levelProgress;
	layer.images[MONEY_BACKGROUND_INDEX] = moneyBackground;
	layer.images[QUEST_BUTTON_INDEX_IN_LAYER] = questButtonImage;
	layer.images[SETTING_BUTTON_INDEX_IN_LAYER] = settingButtonImage;
	layer.imageCount = MAP_IMAGE_COUNT;
	layer.renderAll(&layer);

	printText(layer._consoleDC, 1450, 90, 50, 0, RGB(255, 255, 255), TA_RIGHT, "원");

	layer.applyToDC = applyUserValuesToDC;
}

void updateBuildingState() {
	if (!isFileExist(DIR_BUILDING_STATE)) {
		saveBuildingState(0, 0, 0);
	}
	loadBuildingState(&isFirstOfficeEnabled, &isMyBuildingEnabled, &isCasinoEnabled);
	const int offset = BUILDING_BUTTON_INDEX_OFFSET;

	if (isFirstOfficeEnabled) {
		buttons[FIRST_OFFICE_INDEX + offset].normal = FILE_FIRST_OFFICE;
		buttons[FIRST_OFFICE_INDEX + offset].hovered = FILE_FIRST_OFFICE_HOVER;
		buttons[FIRST_OFFICE_INDEX + offset].clicked = FILE_FIRST_OFFICE_CLICK;
	}
	else {
		buttons[FIRST_OFFICE_INDEX + offset].normal = FILE_FIRST_OFFICE_LOCKED;
		buttons[FIRST_OFFICE_INDEX + offset].hovered = FILE_FIRST_OFFICE_LOCKED;
		buttons[FIRST_OFFICE_INDEX + offset].clicked = FILE_FIRST_OFFICE_LOCKED;
	}
	if (isMyBuildingEnabled) {
		buttons[MY_BUILDING_INDEX + offset].normal = FILE_MY_BUILDING;
		buttons[MY_BUILDING_INDEX + offset].hovered = FILE_MY_BUILDING_HOVER;
		buttons[MY_BUILDING_INDEX + offset].clicked = FILE_MY_BUILDING_CLICK;
	}
	else {
		buttons[MY_BUILDING_INDEX + offset].normal = FILE_MY_BUILDING_LOCKED;
		buttons[MY_BUILDING_INDEX + offset].hovered = FILE_MY_BUILDING_LOCKED;
		buttons[MY_BUILDING_INDEX + offset].clicked = FILE_MY_BUILDING_LOCKED;
	}
	if (isCasinoEnabled) {
		buttons[CASINO_INDEX + offset].normal = FILE_CASINO;
		buttons[CASINO_INDEX + offset].hovered = FILE_CASINO_HOVER;
		buttons[CASINO_INDEX + offset].clicked = FILE_CASINO_CLICK;
	}
	else {
		buttons[CASINO_INDEX + offset].normal = FILE_CASINO_LOCKED;
		buttons[CASINO_INDEX + offset].hovered = FILE_CASINO_LOCKED;
		buttons[CASINO_INDEX + offset].clicked = FILE_CASINO_LOCKED;
	}
}

void beginMapScreen() {
	void initMapScreen(Image*);
	Image images[MAP_IMAGE_COUNT];
	initMapScreen(images);

	getCompanyNameIfNotExist();
	char companyName[100];
	loadCompanyName(companyName);

	initMapUI();
	updateUserValues();

	startButtonListener(buttons, MAP_BUTTON_COUNT, &layer);
}

void initMapScreen(Image* images) {
	const Button firstOffice = createButton(370, 370, FILE_FIRST_OFFICE, FILE_FIRST_OFFICE_HOVER, FILE_FIRST_OFFICE_CLICK, FIRST_OFFICE_INDEX, onButtonInMapClicked);
	const Button myBuilding = createButton(800, 400, FILE_MY_BUILDING, FILE_MY_BUILDING_HOVER, FILE_MY_BUILDING_CLICK, MY_BUILDING_INDEX, onButtonInMapClicked);
	const Button estate = createButton(1368, 284, FILE_ESTATE, FILE_ESTATE_HOVER, FILE_ESTATE_CLICK, ESTATE_INDEX, onButtonInMapClicked);
	const Button casino = createButton(2094, 688, FILE_CASINO, FILE_CASINO_HOVER, FILE_CASINO_CLICK, CASINO_INDEX, onButtonInMapClicked);
	const Button expDetailCollider = createCollider(455, 65, FILE_LEVEL_PROGRESS_DEFAULT, onButtonInMapHovered);

	int offset = BUILDING_BUTTON_INDEX_OFFSET;
	buttons[FIRST_OFFICE_INDEX + offset] = firstOffice;
	buttons[MY_BUILDING_INDEX + offset] = myBuilding;
	buttons[ESTATE_INDEX + offset] = estate;
	buttons[CASINO_INDEX + offset] = casino;
	buttons[EXP_DETAIL_COLLIDER_INDEX] = expDetailCollider;

	updateBuildingState();

	images[MAP_INDEX] = (Image){ FILE_MAP, 0, 0 };
	images[FIRST_OFFICE_INDEX] = (Image){ buttons[FIRST_OFFICE_INDEX + offset].normal,firstOffice.x, firstOffice.y };
	images[MY_BUILDING_INDEX] = (Image){ buttons[MY_BUILDING_INDEX + offset].normal, myBuilding.x,myBuilding.y };
	images[ESTATE_INDEX] = (Image){ buttons[ESTATE_INDEX + offset].normal, estate.x, estate.y };
	images[CASINO_INDEX] = (Image){ buttons[CASINO_INDEX + offset].normal, casino.x, casino.y };

	layer.images = images;
	layer.imageCount = 5;
	layer.fadeIn(&layer, NULL);
}

void printText(HDC hdc, int x, int y, int size, int weight, COLORREF textColor, int align, char* text) {
	if (weight == 0) weight = 900;
	size = (int)(size * RESOLUTION_MULTIPLIER);
	const HFONT font = CreateFont(size, 0, 0, 0, weight, 0, 0, 0, HANGEUL_CHARSET,
		0, 0, 0, VARIABLE_PITCH | FF_ROMAN, TEXT("둥근모꼴"));

	SelectObject(hdc, font);
	SetBkMode(hdc, TRANSPARENT);
	SetTextColor(hdc, textColor);
	SetTextAlign(hdc, align);

	x = (int)(x * RESOLUTION_MULTIPLIER);
	y = (int)(y * RESOLUTION_MULTIPLIER);
	TextOut(hdc, x, y, text, lstrlen(text));

	PAINTSTRUCT paint;
	EndPaint(WINDOW_HANDLE, &paint);

	DeleteObject(font);
}

void textPositionTester(int size, int weight, COLORREF textColor, int align, char* text) {
	while (1) {
		if (isMouseClicked()) {
			const COORD position = getMousePosition();
			layer.renderAll(&layer);
			printText(layer._consoleDC, position.X, position.Y, size, weight, textColor, align, text);

			gotoXY(0, 0);
			printf("%5d %5d", position.X, position.Y);
		}
	}
}

Button createButton(int x, int y, char* normal, char* hovered, char* clicked, int indexOfLayer, void (*onClick)(Button*)) {
	Button button = DEFAULT_BUTTON;
	button.x = x;
	button.y = y;
	button.normal = normal;
	button.hovered = hovered;
	button.clicked = clicked;
	button.initialize(&button);
	button.indexOfImageLayer = indexOfLayer;
	button.onClick = onClick;
	return button;
}

Button createCollider(int x, int y, char* normal, void(*onHover)(Button*)) {
	Button button = DEFAULT_BUTTON;
	button.x = x;
	button.y = y;
	button.normal = normal;
	button.hovered = NULL;
	button.clicked = NULL;
	button.initialize(&button);
	button.indexOfImageLayer = -1;
	button.onHover = onHover;
	return button;
}

void gotoXY(int x, int y) {
	gotoxy((COORD) { x, y });
}

void initLayer() {
	layer = DEFAULT_IMAGE_LAYER;
	layer.initialize(&layer);
	layer.transparentColor = RGB(0, 255, 25);
}