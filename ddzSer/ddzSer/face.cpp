#include"pch.h"
#include "include/curl/curl.h"
#include "include/json/json.h"
#include"face.h"

const static std::string faceDetect_request_url = "https://aip.baidubce.com/rest/2.0/face/v3/detect";
static std::string faceDetect_result;
using namespace std;

/**
 * 人脸检测与属性分析
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceDetect(const std::string& access_token, const std::string& imageStr, size_t (*pFaceDetectCallback)(void*, size_t , size_t, void*),sLoginSock* lthis) {
	std::string url = faceDetect_request_url + "?access_token=" + access_token;
	CURL* curl = NULL;
	CURLcode result_code;
	int is_success;
	std::string dataStr = "{\"image\":\"";
	dataStr += imageStr;
	dataStr += "\",\"image_type\":\"BASE64\",\"face_field\":\"faceshape,facetype\"}";

	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pFaceDetectCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)lthis);
		result_code = curl_easy_perform(curl);
		if (result_code != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result_code));
			is_success = 1;
			return is_success;
		}
		curl_easy_cleanup(curl);
		is_success = 0;
	}
	else {
		fprintf(stderr, "curl_easy_init() failed.");
		is_success = 1;
	}
	return is_success;
}

const static std::string faceAddRequest_url = "https://aip.baidubce.com/rest/2.0/face/v3/faceset/user/add";
static std::string add_result;
/**
 * 人脸注册
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int addFace(const std::string& access_token,const std::string& imageStr,const std::string account, size_t (*pAddFaceCallback)(void*, size_t,size_t,void*), sLoginSock* lthis) {
	std::string url = faceAddRequest_url + "?access_token=" + access_token;
	CURL* curl = NULL;
	CURLcode result_code;
	int is_success;
	std::string dataStr = "{\"image\":\"";
	dataStr += imageStr;
	dataStr += "\",\"image_type\":\"BASE64\",\"group_id\":\"UserGroup1\",\"user_id\":\"";
	dataStr += account;
	dataStr += "\",\"user_info\":\"abc\",\"quality_control\":\"LOW\",\"liveness_control\":\"NORMAL\"}";
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataStr.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pAddFaceCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)lthis);
		result_code = curl_easy_perform(curl);
		if (result_code != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result_code));
			is_success = 1;
			return is_success;
		}
		curl_easy_cleanup(curl);
		is_success = 0;
	}
	else {
		fprintf(stderr, "curl_easy_init() failed.");
		is_success = 1;
	}
	return is_success;
}

const static std::string faceSearchRequest_url = "https://aip.baidubce.com/rest/2.0/face/v3/search";
/**
 * 人脸搜索
 * @return 调用成功返回0，发生错误返回其他错误码
 */
int faceSearch(const std::string& access_token, const std::string& imageStr, size_t (*pFaceSearchCallback)(void*,size_t,size_t, void*),sLoginSock* lthis) {
	std::string url = faceSearchRequest_url + "?access_token=" + access_token;
	CURL* curl = NULL;
	CURLcode result_code;
	int is_success;
	
	std::string dataStr = "{\"image\":\"";
	dataStr += imageStr;
	dataStr += "\",\"image_type\":\"BASE64\",\"group_id_list\":\"UserGroup1\",\"quality_control\":\"LOW\",\"liveness_control\":\"NORMAL\"}";
	char accounta[ACCOUNT_MAXSIZE + 1];
	wcstombs(accounta, lthis->tui.account, ACCOUNT_MAXSIZE + 1);
	//dataStr += accounta
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.data());
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_slist* headers = NULL;
		headers = curl_slist_append(headers, "Content-Type:application/json;charset=UTF-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataStr);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pFaceSearchCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)lthis);
		result_code = curl_easy_perform(curl);
		if (result_code != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result_code));
			is_success = 1;
			return is_success;
		}
		curl_easy_cleanup(curl);
		is_success = 0;
	}
	else {
		fprintf(stderr, "curl_easy_init() failed.");
		is_success = 1;
	}
	return is_success;
}