/*
 * This software was developed at the National Institute of Standards and
 * Technology (NIST) by employees of the Federal Government in the course
 * of their official duties. Pursuant to title 17 Section 105 of the
 * United States Code, this software is not subject to copyright protection
 * and is in the public domain. NIST assumes no responsibility whatsoever for
 * its use by other parties, and makes no guarantees, expressed or implied,
 * about its quality, reliability, or any other characteristic.
 */

#ifdef Darwin
#include <CommonCrypto/CommonDigest.h>
#include <CoreFoundation/CoreFoundation.h>
#include <Security/Security.h>
#else
#include <openssl/buffer.h>
#include <openssl/evp.h>
#endif

#include <algorithm>
#include <cctype>
#include <locale>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include <be_memory_autoarray.h>
#include <be_sysdeps.h>
#include <be_text.h>

namespace BE = BiometricEvaluation;

std::string
BiometricEvaluation::Text::trimWhitespace(
    const std::string &s,
    const std::locale &locale)
{
	return (Text::ltrimWhitespace(Text::rtrimWhitespace(s)));
}

std::string
BiometricEvaluation::Text::ltrimWhitespace(
    const std::string &s,
    const std::locale &locale)
{
	std::string output{s};

	/* Erase from beginning until the first non-whitespace */
	output.erase(output.begin(),
	    std::find_if(output.begin(), output.end(),
	    [&locale](const char &c) -> bool {
		return (!std::isspace(c, locale));
	    }));

	return (output);
}

std::string
BiometricEvaluation::Text::rtrimWhitespace(
    const std::string &s,
    const std::locale &locale)
{
	std::string output{s};

	/* Erase from the last non-whitespace to the end */
	output.erase(std::find_if(output.rbegin(), output.rend(),
	    [&locale](const char &c) -> bool {
		return (!std::isspace(c, locale));
	    }).base(), output.end());

	return (output);
}

std::string
BiometricEvaluation::Text::trim(
    const std::string &s,
    const char trimChar)
{
	return (ltrim(rtrim(s, trimChar), trimChar));
}

std::string
BiometricEvaluation::Text::ltrim(
    const std::string &s,
    const char trimChar)
{
	std::string output{s};

	/* Erase from beginning until the first non-trimChar */
	output.erase(output.begin(),
	    std::find_if(output.begin(), output.end(),
	    [&trimChar](const char &c) -> bool {
		return (trimChar != c);
	    }));

	return (output);
}

std::string
BiometricEvaluation::Text::rtrim(
    const std::string &s,
    const char trimChar)
{
	std::string output{s};

	/* Erase from the last non-trimChar to the end */
	output.erase(std::find_if(output.rbegin(), output.rend(),
	    [&trimChar](const char &c) -> bool {
		return (trimChar != c);
	    }).base(), output.end());

	return (output);
}

std::string
BiometricEvaluation::Text::digest(
    const void *buffer,
    const size_t buffer_size,
    const std::string &digest)
{
#ifdef Darwin
	/* Use CommonCrypto under OS X (10.4 or later) */
	
 	/* Length of the resulting digest */
	CC_LONG digestLength;
	/* Function pointer to the CommonCrypto digest function */
	unsigned char *
	(*digestFunction)
	    (const void *data,
	    CC_LONG len,
	    unsigned char *md);
	
	/* Determine the digest requested */
	if (strcasecmp(digest.c_str(), "md5") == 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
		digestFunction = CC_MD5;
#pragma clang diagnostic pop
		digestLength = CC_MD5_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "md4") == 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
		digestFunction = CC_MD4;
#pragma clang diagnostic pop
		digestLength = CC_MD4_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "md2") == 0) {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
		digestFunction = CC_MD2;
#pragma clang diagnostic pop
		digestLength = CC_MD2_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "sha1") == 0) {
		digestFunction = CC_SHA1;
		digestLength = CC_SHA1_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "sha224") == 0) {
		digestFunction = CC_SHA224;
		digestLength = CC_SHA224_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "sha256") == 0) {
		digestFunction = CC_SHA256;
		digestLength = CC_SHA256_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "sha384") == 0) {
		digestFunction = CC_SHA384;
		digestLength = CC_SHA384_DIGEST_LENGTH;
	} else if (strcasecmp(digest.c_str(), "sha512") == 0) {
		digestFunction = CC_SHA512;
		digestLength = CC_SHA512_DIGEST_LENGTH;
	} else
		throw Error::NotImplemented(digest);
	
	/* Obtain the digest */
	Memory::AutoArray<unsigned char> md(digestLength);
	if (digestFunction(buffer, buffer_size, md) == nullptr)
		throw Error::StrategyError("Could not obtain digest");
	
	/* Stringify the digest */
	std::stringstream ret;
	for (CC_LONG i = 0; i < digestLength; i++)
		ret << std::hex << std::setw(2) << std::setfill('0')
		    << (int)md[i];
		
	return (ret.str());
#else
	/* Use OpenSSL everywhere else */
	
	/* This need only be called once per executable */
	static bool digests_loaded = false;
	if (!digests_loaded) {
		OpenSSL_add_all_digests();
		digests_loaded = true;
	}

	/* Supports any digest type supported by OpenSSL (MD5, SHA1, ...) */
	const EVP_MD *md;
	md = EVP_get_digestbyname(digest.c_str());
	if (!md)
		throw Error::StrategyError("Unknown message digest: " + digest);

	#if OPENSSL_VERSION_NUMBER < 0x10100000
	EVP_MD_CTX* (*mdctxNewFn)(void) = &EVP_MD_CTX_create;
	void (*mdctxFreeFn)(EVP_MD_CTX*) = &EVP_MD_CTX_destroy;
	#else
	EVP_MD_CTX* (*mdctxNewFn)(void) = &EVP_MD_CTX_new;
	void (*mdctxFreeFn)(EVP_MD_CTX*) = &EVP_MD_CTX_free;
	#endif /* OPENSSL_VERSION_NUMBER */

	std::unique_ptr<EVP_MD_CTX, void(*)(EVP_MD_CTX*)> mdctx(
	    mdctxNewFn(), mdctxFreeFn);

	EVP_DigestInit_ex(mdctx.get(), md, nullptr);
	EVP_DigestUpdate(mdctx.get(), buffer, buffer_size);

	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_size;
	EVP_DigestFinal_ex(mdctx.get(), md_value, &md_size);

	std::stringstream ret;
	for (unsigned int i = 0; i < md_size; i++)
		ret << std::hex << std::setw(2) << std::setfill('0') <<
		    (int)md_value[i];

	return ret.str();
#endif
}

std::string
BiometricEvaluation::Text::digest(
    const std::string &s,
    const std::string &digest)
{
	return (BiometricEvaluation::Text::digest(s.c_str(), s.length(),
	    digest));
}

std::vector<std::string>
BiometricEvaluation::Text::split(
    const std::string &str,
    const char delimiter,
    bool escape)
{
	if (delimiter == '\\')
		throw Error::ParameterError("Cannot use \\ as delimiter");
		
	std::vector<std::string> ret;

	std::string cur_str("");
	for (unsigned int i = 0; i < str.length(); i++)
	{
		if (str[i] == delimiter) {
			/* Don't insert empy tokens */
			if (cur_str == "")
				continue;
			
			/* Check for normal escaped delimiter */
			if (escape && i != 0 && str[i - 1] == '\\') {
				cur_str = cur_str.substr(0,
				    cur_str.length() - 1);
				cur_str.push_back(str[i]);
				continue;
			}
			
			/* Non-escaped delimiter reached: add token */
			ret.push_back(cur_str);
			cur_str = "";
		} else
			cur_str.push_back(str[i]);
	}
	
	/* Add partially formed token if not empty */
	if (cur_str != "")
		ret.push_back(cur_str);

	/* Add the original string if the delimiter was not found */
	if (ret.size() == 0)
		ret.push_back(str);

	return (ret);
}

std::string
BiometricEvaluation::Text::basename(
    const std::string &path)
{
	/* Erase trailing slashes */
	std::string pathCopy{path};
	pathCopy.erase(std::find_if_not(pathCopy.rbegin(), pathCopy.rend(),
	    [](const char &c) -> bool {	return (c == '/'); }).base(),
	    pathCopy.end());
	/* path was only slashes */
	if (pathCopy.length() == 0)
		pathCopy = path;

	static Memory::AutoArray<char> buf;
	buf.resize(pathCopy.length() + 1);
	strncpy(buf, pathCopy.c_str(), pathCopy.length() + 1);

	return (::basename(buf));
}

std::string
BiometricEvaluation::Text::dirname(
    const std::string &path)
{
	static Memory::AutoArray<char> buf;
	buf.resize(strlen(path.c_str()) + 1);
	strncpy(buf, path.c_str(), strlen(path.c_str()) + 1);

	return (::dirname(buf));
}

bool
BiometricEvaluation::Text::caseInsensitiveCompare(
    const std::string &str1,
    const std::string &str2)
{
	/* Enumerate character by character */
	return ((str1.size() == str2.size()) &&
	    std::equal(str1.cbegin(), str1.cend(), str2.cbegin(),
	    [](const char str1Char, const char str2Char) {
	        return (std::toupper(str1Char) == std::toupper(str2Char));
	    })
	);
}

std::string
BiometricEvaluation::Text::toUppercase(
    const std::string &str,
    const std::locale &locale)
{
	const auto &facet = std::use_facet<
	    std::ctype<std::string::value_type>>(locale);

	std::string retStr{str};
	facet.toupper(&retStr[0], &retStr[0] + retStr.size());
	return (retStr);
}

std::string
BiometricEvaluation::Text::toLowercase(
    const std::string &str,
    const std::locale &locale)
{
	const auto &facet = std::use_facet<
	    std::ctype<std::string::value_type>>(locale);

	std::string retStr{str};
	facet.tolower(&retStr[0], &retStr[0] + retStr.size());
	return (retStr);
}

std::string
BiometricEvaluation::Text::encodeBase64(
    const BiometricEvaluation::Memory::uint8Array &data)
{
#ifdef Darwin
	CFErrorRef error = nullptr;

	SecTransformRef transform = SecEncodeTransformCreate(
	    kSecBase64Encoding, &error);
	if (error != nullptr) {
		CFRelease(transform);
		throw BE::Error::StrategyError("SecEncodeTransformCreate(): " +
		    std::to_string(CFErrorGetCode(error)));
	}

	/* Convert to CFString */
	CFDataRef sourceData = static_cast<CFDataRef>(CFDataCreate(
	    kCFAllocatorDefault, data, data.size() + 1));

	/* Set data to be encoded */
	SecTransformSetAttribute(transform, kSecTransformInputAttributeName,
	    sourceData, &error);
	CFRelease(sourceData);
	if (error != nullptr) {
		CFRelease(transform);
		throw BE::Error::StrategyError("SecTransformSetAttribute(): " +
		    std::to_string(CFErrorGetCode(error)));
	}

	/* Encode */
	auto encodedData = static_cast<CFDataRef>(SecTransformExecute(
	    transform, &error));
	CFRelease(transform);
	if (error != nullptr)
		throw BE::Error::StrategyError("SecTransformExecute(): " +
		    std::to_string(CFErrorGetCode(error)));

	/* Convert result to CFString */
	auto stringRef = CFStringCreateFromExternalRepresentation(
	    kCFAllocatorDefault, encodedData, kCFStringEncodingASCII);
	CFRelease(encodedData);

	/* Convert CFString to C string */
	auto bufferSize = CFStringGetLength(stringRef) + 1;
	std::unique_ptr<char[]> buffer(new char[bufferSize]);
	auto rv = CFStringGetCString(stringRef, buffer.get(), bufferSize,
	    kCFStringEncodingASCII);
	CFRelease(stringRef);
	if (!rv)
		throw BE::Error::StrategyError("CFStringGetCString");

	return (std::string(buffer.get(), bufferSize));
#else
	BIO *handle = BIO_new(BIO_s_mem());
	handle = BIO_push(BIO_new(BIO_f_base64()), handle);

	/* One line output */
	BIO_set_flags(handle, BIO_FLAGS_BASE64_NO_NL);

	/* Encode */
	BIO_write(handle, data, data.size());
	(void)BIO_flush(handle);

	BUF_MEM *buffer;
	BIO_get_mem_ptr(handle, &buffer);

	std::string encodedString(buffer->data, buffer->length);
	BIO_free_all(handle);

	return (encodedString);
#endif /* Darwin */
}

BiometricEvaluation::Memory::uint8Array
BiometricEvaluation::Text::decodeBase64(
    const std::string &data)
{
#ifdef Darwin
	CFErrorRef error = nullptr;

	SecTransformRef transform = SecDecodeTransformCreate(
	    kSecBase64Encoding, &error);
	if (error != nullptr) {
		CFRelease(transform);
		throw BE::Error::StrategyError("SecDecodeTransformCreate(): " +
		    std::to_string(CFErrorGetCode(error)));
	}

	/* Convert to CFData */
	CFDataRef sourceData = static_cast<CFDataRef>(CFDataCreate(
	    kCFAllocatorDefault, (uint8_t *)data.c_str(),
	    data.size() + 1));

	/* Set data to be decoded */
	SecTransformSetAttribute(transform, kSecTransformInputAttributeName,
	    sourceData, &error);
	CFRelease(sourceData);
	if (error != nullptr) {
		CFRelease(transform);
		throw BE::Error::StrategyError("SecTransformSetAttribute(): " +
		    std::to_string(CFErrorGetCode(error)));
	}

	/* Decode */
	CFDataRef decodedData = static_cast<CFDataRef>(SecTransformExecute(
	    transform, &error));
	CFRelease(transform);
	if (error != nullptr)
		throw BE::Error::StrategyError("SecTransformExecute(): " +
		    std::to_string(CFErrorGetCode(error)));

	/* Convert to AutoArray */
	BE::Memory::uint8Array aa(CFDataGetLength(decodedData));
	CFDataGetBytes(decodedData, CFRangeMake(0, aa.size()), aa);
	CFRelease(decodedData);

	return (aa);
#else
	if (data.find_first_of('\n') != std::string::npos)
		throw BE::Error::NotImplemented("Newlines in encoded data");

	BIO *handle = BIO_new_mem_buf(const_cast<char *>(data.c_str()), -1);
	handle = BIO_push(BIO_new(BIO_f_base64()), handle);

	/* One line input */
	BIO_set_flags(handle, BIO_FLAGS_BASE64_NO_NL);

	/* TODO: Avoid over-allocation */
	BE::Memory::uint8Array decodedData(data.size());

	const auto len = BIO_read(handle, decodedData, decodedData.size());
	decodedData.resize(len);

	BIO_free_all(handle);

	return (decodedData);
#endif /* Darwin */
}
