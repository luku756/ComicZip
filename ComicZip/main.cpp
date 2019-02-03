#include "header.h"

int main() {

	findFileList("","");

}

void findFileList(string path, string name) {


	HANDLE hSearch;
	WIN32_FIND_DATA wfd;
	bool bResult = true;

	int dir_count = 0; //�� ���� �ȿ� ���丮�� �� �� �ִ���. 
	int image_count = 0;	//�� ���� �ȿ� �̹��� ������ �� �� �ִ���. �̹����� ���� ������ leaf �� ���� X.

	//��� ���� �˻�
	hSearch = FindFirstFile((path + "*").c_str(), &wfd);	//���� ��ο�������.
	do {
		string str = string(wfd.cFileName);
		string format;	//Ȯ���� ����
		int index = str.find_last_of(".");
		if (index > 0) {	//������ �Ϻ� ��� Ȯ���� ����
			format = str.substr(str.find_last_of(".") + 1);

			if (format == "jpg" || format == "gif" || format == "jpeg" || format == "png" || format == "bmp" || format == "tif" || format == "tiff") {
				image_count++;
			}
		}

		if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && (strcmp(".", wfd.cFileName) != 0) && (strcmp("..", wfd.cFileName) != 0)) {//������. ���糪 ���� ������ X.
			dir_count++;
			findFileList(path + str + "\\", str);
		}
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	if (dir_count == 0 && image_count > 0) {//leaf node. ���ʿ� ���� ����.
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
	path = path.substr(0, path.length() - 1);	//\\ ����
	sprintf(dirPath, "%s", path.c_str());
	sprintf(zipPath, "%s.zip", path.c_str(), name.c_str());

	len = strlen(targetPath);

	//zip����
	hz = CreateZip(zipPath, 0);


	//��� ���� �˻�
	hSearch = FindFirstFile(searchPath, &wfd);
	do {
		//����(�������� �ȿ� �߰�)
		sprintf(&targetPath[len], "%s", wfd.cFileName);
		ZipAdd(hz, wfd.cFileName, targetPath);
		DeleteFile(targetPath);
		bResult = FindNextFile(hSearch, &wfd);
	} while (bResult);
	FindClose(hSearch);

	//��������
	CloseZip(hz);

	//���� ����
	RemoveDirectory(dirPath);

}
