#include "header.h"

int main() {

	findFileList("","");

}

void findFileList(string path, string name) {


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

			if (format == "jpg" || format == "gif" || format == "jpeg" || format == "png" || format == "bmp" || format == "tif" || format == "tiff") {
				image_count++;
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//폴더만. 현재나 상위 폴더는 X.
			dir_count++;
			findFileList(path + str + "\\", str);
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. 안쪽에 폴더 없음.
		printf("find comic folder. zip %s\n",name.c_str());
		zipFolder(path, name);
	}

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
