/*
 * 제작 : 민승현(개발), 이혜원(디자인)
 * https://github.com/MinSeungHyun/console-game
 * 게임 이름 : Code The Company
 * 직접 회사를 경영하고, 키워갈 수 있는 게임이다.
 * 이름과 회사 이름을 입력하는 부분을 제외하고 모든 조작은 마우스로만 이루어지며,
 * 클릭할 수 있는 부분은 마우스를 올리면 강조된다.
 */

#include "Defines.h"

void beginMapScreen(int);
void beginEstateScreen();

//게임의 메인 진행이다. (나중에 게임 다시시작할 때 필요해서 분리했다.)
void mainProcess() {
	initialize();
	_mkdir(DIR_SAVE);
	initLayer();
	initQuests();
	initEstateItems();
	Sleep(300);

	if (ENABLE_DEVELOPMENT_MODE) {
		beginMapScreen(0);
	}
	else {
		beginStartScreen();
		Sleep(500);

		if (!isFileExist(DIR_NAME)) {
			beginEnterUserNameScreen();
			Sleep(500);
		}
		loadName(lastName, firstName);

		if (!isFileExist(DIR_COMPANY_NAME)) {
			beginStoryScreen();
			Sleep(1000);
		}
		beginMapScreen(1);
	}
}

/*
 * mainProcess함수를 실행시킨다.
 * mainProcess에서는 게임을 하기 위한 기본 설정을 한 뒤, 시작 화면부터 차근차근 진행되는데,
 * beginMapScreen으로 메인 화면이 띄어지게된 후 부터는 사용자의 입력에 맞게 진행되어야 하므로
 * 메인 함수에서 게임이 진행되는 것이 아니라 각각의 화면에서 사용자의 입력에 맞게 진행된다.
 */
int main() {
	mainProcess();
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

//회사 이름 입력받는 화면을 초기화함
void initGetCompanyNameScreen() {
	layer.images[5] = (Image){ FILE_QUEST_WINDOW_NO_TITLE, 730, 380 };
	layer.imageCount = 6;
	layer.renderAll(&layer);

	printText(layer._consoleDC, 1440, 500, 70, 0, QUEST_TEXT_COLOR, TA_CENTER, "회사 이름을 입력해주세요");
}

//회사 이름 입력받는 화면 시작
void getCompanyNameIfNotExist() {
	if (isFileExist(DIR_COMPANY_NAME)) return;
	initGetCompanyNameScreen();

	char companyName[100];
	companyName[0] = '\0';
	int i = 0;
	while (1) {
		const int input = _getch();

		if (input == '\r') {
			if (companyName[0] == '\0') continue;
			break;
		}
		if (input == '\b') {
			if (companyName[i - 1] < -1) i -= 3;
			else i -= 2;
			if (i < -1) i = -1;
		}
		else if (i >= COMPANY_NAME_MAX_LENGTH || input == ' ' || input == '\\') continue;
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

//돈과, 경험치, 초당 들어오는 돈을 갱신함
void updateUserValues() {
	loadMoneyAndExp(&money, &userExp);
	level = (int)sqrtl((long double)(userExp / 400));

	loadMPS(&mps);
}

//현재 레벨의 경험치를 반환함
BigInt getExpForLevel(int level) {
	return (BigInt)(400 * (pow(level, 2)));
}

//현재 레벨에서 다음 레벨까지 가기 위한 경험치를 반환함
BigInt getTotalExpForLevel(int level) {
	return getExpForLevel(level + 1) - getExpForLevel(level);
}

//현재 레벨에서 다음레벨까지 가기 위해 모은 경험치를 반환함
BigInt getAchievedExp() {
	return userExp - getExpForLevel(level);
}

//현재 레벨부터 다음 레벨까지의 진행도를 0부터 10까지 중에 반환해줌
int getProgressFromExp() {
	const BigInt totalExp = getTotalExpForLevel(level);
	const BigInt achievedExp = getAchievedExp();
	return (int)((long double)achievedExp / totalExp * 10);
}

//메인화면에서 경험치바 위에 마우스를 올리면 호출되어 수치를 나타내주는 함수
void displayExpDetail(HDC hdc) {
	char expText[100];
	sprintf(expText, "%lld/%lld", getAchievedExp(), getTotalExpForLevel(level));
	printText(hdc, 540, 100, 50, 10, RGB(255, 255, 255), TA_LEFT, expText);
}

int isExpDetailShow = 0;
//메인화면에서 화면에 글씨를 출력하기 위해 호출되는 함수
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

	printTextWithAngle(hdc, 2000, 1280, 70, 0, -100, RGB(0, 0, 0), TA_CENTER, companyName);
}

Button buttons[MAP_BUTTON_COUNT];

//레벨바에 마우스를 올렸을 때 호출되는 함수
void onButtonInMapHovered(Button* hoveredButton) {
	if (hoveredButton->normal == FILE_LEVEL_PROGRESS_DEFAULT) isExpDetailShow = 1;
}

//메인 화면에 있는 버튼을 클릭했을 때 호출되는 함수
void onButtonInMapClicked(Button* clickedButton) {
	playSound(SOUND_BUTTON_CLICK);
	char* clickedButtonName = clickedButton->normal;
	if (clickedButtonName == FILE_ESTATE) {
		updateQuestActiveState(&quests[0], level);
		if (quests[0].isActivated) {
			quests[0].progress++;
			saveQuestsProgress(quests[0]);
			updateQuestActiveState(&quests[0], level);
			beginQuestCompleteScreen(0);
		}
		else {
			beginEstateScreen();
		}
	}
	else if (clickedButtonName == FILE_QUEST_BUTTON) {
		beginQuestScreen();
	}
	else if (clickedButtonName == FILE_SETTING_BUTTON) {
		beginSettingScreen();
	}
}

//메인 화면에서 UI를 초기화 하기 위한 함수 
void initMapUI() {
	if (!isFileExist(DIR_MONEY_AND_EXP))
		saveMoneyAndExp(DEFAULT_MONEY, DEFAULT_EXP);
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

//건물들의 잠금 해제 상태를 업데이트 하는 함수
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

//메 초마다 호출되어 돈을 증가시키는 함수
void onEverySecond(void* cnt) {
	money += mps;
	saveMoneyAndExp(money, userExp);
}

//메인 화면을 초기화 하는 함수
void initMapScreen(Image* images, int isFirstShow) {
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
	if (isFirstShow) layer.fadeIn(&layer, NULL);
	else layer.renderAll(&layer);
}

//메인 화면을 시작하는 함수
void beginMapScreen(int isFirstShow) {
	stopButtonListener();
	layer.applyToDC = NULL;
	Image images[MAP_IMAGE_COUNT];
	initMapScreen(images, isFirstShow);

	getCompanyNameIfNotExist();
	loadCompanyName(companyName);

	const int previousLevel = level;
	initMapUI();
	updateUserValues();
	if (!isFirstShow && level != previousLevel) {
		beginLevelUpScreen();
	}
	else {
		if (isFirstShow) {
			startSecondClock(onEverySecond);
			playBGM(SOUND_MAIN_BGM);
		}

		startButtonListener(buttons, MAP_BUTTON_COUNT, &layer);
	}
}

#define BACK_BUTTON_INDEX_OF_LAYER 1
#define ESTATE_BUTTON_COUNT 2
#define ESTATE_IMAGE_COUNT 5

//부동산 화면에 있는 버튼을 클릭했을 때 호출됨
void onButtonInEstateClicked(Button* button) {
	playSound(SOUND_BUTTON_CLICK);
	char* clickedButtonName = button->normal;
	if (clickedButtonName == FILE_BACK_BUTTON) {
		beginMapScreen(0);
	}
	else if (clickedButtonName == FILE_ESTATE_BUY_BUTTON) {
		beginEstateBoughtScreen(0);
	}
}

//부동산 화면에서 글씨를 출력하기 위해 호출되는 함수
void applyToDcInEstate(HDC hdc) {
	char firstOfficePrice[100] = "이미 구매함";
	if (!isFirstOfficeEnabled) sprintf(firstOfficePrice, "%lld원", estateItems[0].price);
	printText(hdc, 1440, 1220, 80, 0, RGB(255, 255, 255), TA_CENTER, firstOfficePrice);
	printText(hdc, 1440, 950, 60, 0, RGB(0, 0, 0), TA_CENTER, estateItems[0].itemName);
}

//부동산 화면을 시작하는 함수
void beginEstateScreen() {
	stopButtonListener();

	const Button backButton = createButton(100, 1280, FILE_BACK_BUTTON, FILE_BACK_BUTTON_HOVER, FILE_BACK_BUTTON_CLICK, BACK_BUTTON_INDEX_OF_LAYER, onButtonInEstateClicked);
	Button buyButton;

	if (isFirstOfficeEnabled)
		buyButton = createButton(1176, 1180, FILE_ESTATE_BOUGHT_BUTTON, FILE_ESTATE_BOUGHT_BUTTON, FILE_ESTATE_BOUGHT_BUTTON, 4, onButtonInEstateClicked);
	else
		buyButton = createButton(1176, 1180, FILE_ESTATE_BUY_BUTTON, FILE_ESTATE_BUY_BUTTON_HOVER, FILE_ESTATE_BUY_BUTTON_CLICK, 4, onButtonInEstateClicked);

	Button buttons[ESTATE_BUTTON_COUNT] = { backButton, buyButton };

	Image images[ESTATE_IMAGE_COUNT] = {
		{FILE_ESTATE_BACKGROUND, 0, 0},
		{backButton.normal, backButton.x, backButton.y},
		{FILE_ESTATE_ITEM_BACKGROUND_UNLOCKED, 1090, 614},
		{FILE_FIRST_OFFICE, 1270, 620},
		{buyButton.normal, buyButton.x, buyButton.y} //4
	};
	layer.images = images;
	layer.imageCount = ESTATE_IMAGE_COUNT;
	layer.applyToDC = applyToDcInEstate;
	layer.renderAll(&layer);

	startButtonListener(buttons, ESTATE_BUTTON_COUNT, &layer);
}