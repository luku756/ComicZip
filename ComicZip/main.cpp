#include "header.h"

#define MODE_ALL 1
#define MODE_SPECIFIC 2

//드래그-드랍 없이 사용할 경우 모든 폴더를 탐색(MODE_ALL)
//드래그-드랍으로 특정 폴더만 탐색할 경우(MODE_SPECIFIC)
int mode = MODE_ALL;

int main(int argc, char* argv[]) {

	if (argc == 1) {//추가 파일 드래그-드랍 없음
		printf("run for here.\n");
		findFileList("", "");
	}
	else {//폴더 드래그-드랍 시 해당 폴더 내용을 탐색. 건드리면 안 되는 파일(임시파일 등)이 섞여있을 때 원하는 폴더만 압축.
		mode = MODE_SPECIFIC;
		for (int i = 1; i < argc; i++) {
			printf("run for there.\n%s\n",argv[i]);
			string path = argv[i];
			findFileList(path + "\\", "");//폴더가 아닐 경우 해당 위치에 파일이 없으니 무시되지만, 폴더일 경우 탐색된다.
			findBadName(path + "\\", "");
		}
	}

}

//불편한 이름일 가능성 (-2화 , .5화) 확인
bool badNameCheck(string filename) {

	int sub = filename.find("-2");

	if (sub == -1) {
		sub = filename.find(".5");
	}

	//printf("filename : %s, sub : %d\n",filename.c_str(),sub);

	if (sub == -1)//괜찮은 이름
		return false;
	else//불편한 이름
		return true;
}

//불편한 이름일 가능성을 확인(7-2화, 52.5화 등)하고, 실제로 (7-1화 대신 7화, 52.0화 대신 52화 등) 인지 확인하고 이름을 바꾼다.
void nameChange(vector<string> allNameList, vector<string> badNameList, string path) {

	string frontName; //불편한 이름
	string newName;	//변경될 편안한 이름
	int sub;
	int type = 0;	//-2인지 .5인지

	for (int i = 0; i < badNameList.size(); i++) {
		sub = badNameList[i].find("-2");
		type = 1;

		if (sub == -1) {
			sub = badNameList[i].find(".5");
			type = 2;
		}

		//printf("bad name %s \n", badNameList[i].c_str());
		frontName = badNameList[i].substr(0, sub) + badNameList[i].substr(sub + 2);	//이 이름이 있다면 변경해야 함
		newName = badNameList[i].substr(0, sub);

		if (type == 1)
			newName += "-1";
		else
			newName += ".0";

		newName += badNameList[i].substr(sub + 2);
		for (int j = 0; j < allNameList.size(); j++) {		

			if (allNameList[j] == frontName) {//불편한 이름이 있는지 확인
				//printf("-------------bad name %s -> %s \n", allNameList[j].c_str(), newName.c_str());
				sub = rename((path + allNameList[j]+".zip").c_str(), (path + newName + ".zip").c_str());
			}

		}


	}

}

//폴더 앞에 붙은 작가명을 삭제. [~~]
//작가명이 아닌데 앞에 붙는 경우는 없다고 가정.
void removeArtistName(string path, string& name) {
	int end = name.find("]");
	if (end > 0) {
		int st = name.find("[");
		if (st == 0) {
			string st = name.substr(end + 2);
			printf("Rename Derectory : %s -> %s\n", name.c_str(), st.c_str());
			string s1 = path + name;
			string s2 = path + st;

			int s = rename((path + name).c_str(), (path + st).c_str());
			name = st;
		}
	}
}

//path : 경로, name : 폴더명(만화 폴더 이름 - xxx24화 등)
string findFileList(string path, string name) {

	string returnName;
	vector<string> allNameList;	//전체 파일 이름 명단.
	vector<string> badNameList;	//불편한 파일 이름 명단.

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	bool bResult = true;

	int dir_count = 0; //이 폴더 안에 디렉토리가 몇 개 있는지. 
	int image_count = 0;	//이 폴더 안에 이미지 파일이 몇 개 있는지. 이미지가 없는 폴더는 leaf 라도 압축 X.

	//모든 파일 검색
	hSearch = FindFirstFile((path + "*").c_str(), &wfd);	//현재 경로에서부터.
	do {
		string str = string(wfd.cFileName);
		string format;	//확장자 검출
		int index = str.find_last_of(".");
		if (index > 0) {	//폴더나 일부 경우 확장자 없음
			format = str.substr(str.find_last_of(".") + 1);
			transform(format.begin(), format.end(), format.begin(), tolower);//소문자화. 파일명이 .JPEG 일 경우를 대비


			if (format == "jpg" || format == "gif" || format == "jpeg" || format == "png" || format == "bmp" || format == "tif" || format == "tiff") {
				image_count++;
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//폴더만. 현재나 상위 폴더는 X. 폴더는 재귀로 들어간다
			dir_count++;
			removeArtistName(path,str);
			returnName = findFileList(path + str + "\\", str);

			if (mode == MODE_ALL) {//MODE_SPECIFIC 의 경우, 후에 따로 불편한 이름을 탐색한다. 중복 탐색 방지
				allNameList.push_back(returnName);

				if (badNameCheck(returnName)) {//불편한 이름일 때
					badNameList.push_back(returnName);
				}
			}
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. 안쪽에 폴더 없음.
		printf("find comic folder. zip %s\n",name.c_str());
		
		zipFolder(path, name);
	}

	if (mode == MODE_ALL) {
		nameChange(allNameList, badNameList, path);	//불편한 이름 (-2화 , .5화) 변경
	}
	return name;
}

//이미 기존에 존재하던 폴더를 보고, 불편한 이름이 있을 경우 찾아서 변경한다.
string findBadName(string path, string name) {

	string returnName;
	vector<string> allNameList;	//전체 파일 이름 명단.
	vector<string> badNameList;	//불편한 파일 이름 명단.

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	bool bResult = true;

	//모든 파일 검색
	hSearch = FindFirstFile((path + "*").c_str(), &wfd);	//현재 경로에서부터.
	do {
		string str = string(wfd.cFileName);
		string format;	//확장자 검출
		int index = str.find_last_of(".");
		if (index > 0) {	//폴더나 일부 경우 확장자 없음
			format = str.substr(str.find_last_of(".") + 1);
			transform(format.begin(), format.end(), format.begin(), tolower);//소문자화. 파일명이 .ZIP 일 경우를 대비
			
			if (format == "zip") {
				string filename = str.substr(0, str.size() - 4);	//.zip을 뺀 이름
				allNameList.push_back(filename);

				if (badNameCheck(filename)) {//불편한 이름일 때
					badNameList.push_back(filename);
				}
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//폴더만. 현재나 상위 폴더는 X. 폴더는 재귀로 들어간다
			findBadName(path + str + "\\", str);
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);
	
	nameChange(allNameList, badNameList, path);	//불편한 이름 (-2화 , .5화) 변경

	return name;
}



void zipFolder(string path, string name) {

	char searchPath[500], zipPath[500], dirPath[500], targetPath[500], filePath[500];

	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	HZIP hz;
	bool bResult = true;
	int len;


	sprintf(searchPath, "%s*", path.c_str());
	sprintf(targetPath, "%s", path.c_str());
	path = path.substr(0, path.length() - 1);	//\\ 제거
	sprintf(dirPath, "%s", path.c_str());
	sprintf(zipPath, "%s.zip", path.c_str(), name.c_str());

	len = strlen(targetPath);

	//zip제작
	hz = CreateZip(zipPath, 0);

	int flag_last = 0;
	//모든 파일 검색
	hSearch = FindFirstFile(searchPath, &wfd);
	do {
		if (flag_last != 0) {
			ZipAdd(hz, wfd.cFileName, targetPath);
		}
		flag_last = 0;
		//압축(압축폴더 안에 추가)
		sprintf(&targetPath[len], "%s", wfd.cFileName);

		int fsize = (wfd.nFileSizeHigh * (MAXDWORD + 1)) + wfd.nFileSizeLow;
		if (fsize != 16293) {
			ZipAdd(hz, wfd.cFileName, targetPath);
			flag_last = 1;
		}
		DeleteFile(targetPath);
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);



	FindClose(hSearch);

	//압축종료
	CloseZip(hz);

	//폴더 삭제
	RemoveDirectory(dirPath);

}
