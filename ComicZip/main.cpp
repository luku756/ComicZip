#include "header.h"

int main() {

	findFileList("","");

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
			returnName = findFileList(path + str + "\\", str);

			allNameList.push_back(returnName);

			if (badNameCheck(returnName)) {//불편한 이름일 때
				badNameList.push_back(returnName);
			}

		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. 안쪽에 폴더 없음.


		printf("find comic folder. zip %s\n",name.c_str());
		zipFolder(path, name);
	}

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


	//모든 파일 검색
	hSearch = FindFirstFile(searchPath, &wfd);
	do {
		//압축(압축폴더 안에 추가)
		sprintf(&targetPath[len], "%s", wfd.cFileName);
		ZipAdd(hz, wfd.cFileName, targetPath);
		DeleteFile(targetPath);
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	//압축종료
	CloseZip(hz);

	//폴더 삭제
	RemoveDirectory(dirPath);

}
