
#include "stdafx.h"
#include <locale>

namespace Spider3d {

	static char cDefaultTableFieldsSplitter = '\t';

	char *readLineFromFile(FILE *fp) {
		int nAllocated, iAllocatedPtr;
		int cRead;
		char *cpAllocated;

		if (feof(fp)) {
			return NULL;
		}

		cpAllocated = (char*)malloc(READ_BUFFER_SIZE);
		if (cpAllocated == NULL) {
			return NULL;
		}
		nAllocated = READ_BUFFER_SIZE;
		iAllocatedPtr = 0;

		while ((cRead = fgetc(fp)) != EOF) {
			if (iAllocatedPtr >= nAllocated - 1) {
				if (nAllocated == 0) {
					cpAllocated = (char*)malloc(READ_BUFFER_SIZE);
				}
				else {
					cpAllocated = (char*)realloc(cpAllocated, nAllocated + READ_BUFFER_SIZE);
				}
				if (cpAllocated == NULL) {
					return NULL;
				}
				nAllocated += READ_BUFFER_SIZE;
			}
			if (cRead == '\n') {
				break;
			}
			cpAllocated[iAllocatedPtr] = (unsigned char)cRead;
			iAllocatedPtr++;
		}
		cpAllocated[iAllocatedPtr] = '\x0';
		return cpAllocated;
	}


	int findTagContent(char *cpText, const char *cpTagName, int iStartAt, int iStopAt, int *ipStart, int *ipEnd) {
		char caOpenTag[MAX_TAG_NAME + 3];
		char caCloseTag[MAX_TAG_NAME + 4];
		int iFound, iOpenTagStart, iOpenTagEnd, iCloseTagStart, iCloseTagEnd;

		if (strlen(cpTagName) > MAX_TAG_NAME) {
			return 0;
		}

		sprintf(caOpenTag, "<%s>", cpTagName);
		sprintf(caCloseTag, "</%s>", cpTagName);

		iFound = findSubstring(cpText, caOpenTag, iStartAt, iStopAt, &iOpenTagStart, &iOpenTagEnd, false);
		if (iFound == 0) {
			return 0;
		}
		iFound = findSubstring(cpText, caCloseTag, iOpenTagEnd, iStopAt, &iCloseTagStart, &iCloseTagEnd, false);
		if (iFound == 0) {
			return 0;
		}

		if (ipStart != NULL) {
			*ipStart = iOpenTagEnd + 1;
		}
		if (ipEnd != NULL) {
			*ipEnd = iCloseTagStart - 1;
		}
		return 1;
	}

	int findSubstring(char *cpText, const char *cpSubstring, int iStartAt, int iStopAt, int *ipStart, int *ipEnd, bool bWholeWordSearch) {
		int iFound = 0;
		int i, iTextLen, iSubstringLen, iSubstringIndex;

		iTextLen = strlen(cpText);
		if (iStopAt == -1) {
			iStopAt = iTextLen - 1;
		}
		iSubstringLen = strlen(cpSubstring);

		iSubstringIndex = 0;
		for (i = iStartAt; i <= iStopAt; i++) {
			if (tolower(cpSubstring[iSubstringIndex]) == tolower(cpText[i])) {
				iSubstringIndex++;
				if (iSubstringIndex == iSubstringLen) {
					if (bWholeWordSearch) {
						if (i != iStopAt) {
							if (cpText[i + 1] != ' ' && cpText[i + 1] != '\t' && cpText[i + 1] != '\r' && cpText[i + 1] != '\n') {
								iSubstringIndex = 0;
								continue;
							}
						}
					}

					if (ipStart != NULL) {
						*ipStart = i - iSubstringLen + 1;
					}
					if (ipEnd != NULL) {
						*ipEnd = i;
					}
					iFound = 1;
					break;
				}
			}
			else {
				if (iSubstringIndex > 0) {
					iSubstringIndex = 0;
				}
			}
		}
		return iFound;
	}

	int getPosByColumnName(char *cpBuffer, const char *cpColumn) {
		int iPosition = -1, i, iStatus, iStart;

		iStatus = findSubstring(cpBuffer, cpColumn, 0, -1, &iStart, NULL);
		if (iStatus == 1) {
			iPosition = 0;
			for (i = 0; i < iStart; i++) {
				if (cpBuffer[i] == cDefaultTableFieldsSplitter) {
					iPosition++;
				}
			}
		}
		return iPosition;
	}

	/*
	std::string& ltrim(std::string& str, const std::string& chars)
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	std::string& rtrim(std::string& str, const std::string& chars)
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	std::string& trim(std::string& str, const std::string& chars)
	{
		//return ltrim(rtrim(str, chars), chars);
		return str;
	}
	*/
	std::string ltrim(std::string& s, const std::string& chars)
	{
		size_t start = s.find_first_not_of(chars);
		return (start == std::string::npos) ? std::string("") : s.substr(start);
	}

	std::string rtrim(std::string& s, const std::string& chars)
	{
		size_t end = s.find_last_not_of(chars);
		return (end == std::string::npos) ? std::string("") : s.substr(0, end + 1);
	}

	std::string trim(std::string& s, const std::string& chars)
	{
		return rtrim(ltrim(s,chars),chars);
	}


	char *trimString(char *cp) {
		for (int i = 0; cp[i] == ' ' && cp[i] != '\x0'; ) {
			for (int j = i; cp[j] != '\x0'; j++) {
				cp[j] = cp[j + 1];
			}
		}
		for (int i = strlen(cp) - 1; (cp[i] == ' ' || cp[i] == '\r' || cp[i] == '\n') && i >= 0; i--) {
			cp[i] = cp[i + 1];
		}
		return cp;
	}


	int timetToStr(time_t timetDT, char *cpBuffer, int iBufferSize, bool bAscTime, bool bHHMM, char cDMYDelim) {
		int iReturn = -1;

		strcpy(cpBuffer, "");

		struct tm *tmDT = localtime(&timetDT);

		if (bAscTime) {
			char *cp = asctime(tmDT);
			if (strlen(cp) < iBufferSize) {
				strcpy(cpBuffer, cp);
				iReturn = 0;
			}
		}
		else if ((tmDT->tm_year > 0 && tmDT->tm_year < 1000) &&
			(tmDT->tm_mon >= 0) && (tmDT->tm_mon <= 11) &&
			(tmDT->tm_mday >= 0) && (tmDT->tm_mday <= 31) &&
			(tmDT->tm_hour >= 0) && (tmDT->tm_hour <= 24) &&
			(tmDT->tm_min >= 0) && (tmDT->tm_min <= 60) && iBufferSize > 28)
		{
			if ((tmDT->tm_hour > 0 || tmDT->tm_min > 0) && bHHMM) {
				sprintf(cpBuffer, "%02d%c%02d%c%4d %02d:%02d",
					tmDT->tm_mday, cDMYDelim, (tmDT->tm_mon + 1), cDMYDelim, (tmDT->tm_year + 1900), tmDT->tm_hour, tmDT->tm_min);
			}
			else {
				sprintf(cpBuffer, "%02d%c%02d%c%4d", tmDT->tm_mday, cDMYDelim, (tmDT->tm_mon + 1), cDMYDelim, (tmDT->tm_year + 1900));
			}
			iReturn = 0;
		}
		return iReturn;
	}

	int parseDatetime(std::string sDatetime, struct tm& tmDatetime) {

		const char *cp = sDatetime.c_str();
		char caDatetime[40];

		int i = 0;
		for (; i < 39 && cp[i] != '\x0'; i++) {
			if (cp[i] == '.' || cp[i] == '-' || cp[i] == ':') {
				caDatetime[i] = ' ';
			}
			else {
				caDatetime[i] = cp[i];
			}
		}
		caDatetime[i] = '\x0';

		int iStatus = sscanf(caDatetime, "%d %d %d %d %d", &tmDatetime.tm_mday, &tmDatetime.tm_mon, &tmDatetime.tm_year, &tmDatetime.tm_hour, &tmDatetime.tm_min);
		if (iStatus != 5) {
			return -1;
		}
		tmDatetime.tm_year -= 1900;
		tmDatetime.tm_mon -= 1;
		tmDatetime.tm_sec = 0;
		tmDatetime.tm_wday = tmDatetime.tm_yday = tmDatetime.tm_isdst = 0;
		return 0;
	}


	int parseFileHeader(FILE *fp, int nFields, char **cpaFields, int **ipaFields, bool allFieldsRequired) {
		int iReturn = 0;
		char *cpHeader;

		cpHeader = readLineFromFile(fp);
		if (cpHeader == NULL) {
			return -1;
		}

		for (int i = 0; i < nFields; i++) {
			int iPos = getPosByColumnName(cpHeader, cpaFields[i]);
			if (iPos == -1 && allFieldsRequired) {
				iReturn = -1;
				break;
			}
			*ipaFields[i] = iPos;
		}

		free(cpHeader);
		return iReturn;
	}

	static int parseFileLineIntoFields(char *cpLine, int nFields, int **ipaFields, int **ipaFieldIndexes, bool allFieldsRequired);

	char *parseFileLine(FILE *fp, int nFields, int **ipaFields, int **ipaFieldIndexes, int *ipStatus, bool allFieldsRequired) {
		char *cpLine;

		cpLine = readLineFromFile(fp);
		if (cpLine == NULL) {
			return NULL;
		}
		*ipStatus = parseFileLineIntoFields(cpLine, nFields, ipaFields, ipaFieldIndexes, allFieldsRequired);
		return cpLine;
	}


	static int parseFileLineIntoFields(char *cpLine, int nFields, int **ipaFields, int **ipaFieldIndexes, bool allFieldsRequired)
	{
		int iLineLen;

		for (int iF = 0; iF < nFields; iF++) {
			*ipaFieldIndexes[iF] = -1;
		}

		iLineLen = strlen(cpLine);
		for (int i = 0, iPos = 0; i < iLineLen; i++) {
			for (int iF = 0; iF < nFields; iF++) {
				if (iPos == *ipaFields[iF] && *ipaFieldIndexes[iF] == -1) {
					*ipaFieldIndexes[iF] = i;
					break;
				}
			}
			if (cpLine[i] == cDefaultTableFieldsSplitter) {
				iPos++;
				cpLine[i] = '\x0';
			}
			else if (cpLine[i] == '\r' || cpLine[i] == '\n') {
				cpLine[i] = '\x0';
			}
		}

		if (allFieldsRequired) {
			for (int iF = 0; iF < nFields; iF++) {
				if (*ipaFieldIndexes[iF] == -1) {
					return -1;
				}
			}
		}
		return 0;
	}

	int parseFileLineIntoFields(const std::string& s, std::vector<std::string>& tokens, char delimiter);

	int parseFileHeader(std::ifstream& infile, std::vector<std::string>& fieldsNames, std::map<std::string, int>& fieldsPositions)
	{
		int numParsed = -1;

		for (int iN = 0; iN < fieldsNames.size(); iN++) {
			fieldsPositions[fieldsNames[iN]] = -1;
		}

		std::string line;
		if (std::getline(infile, line)) {
			std::vector<std::string> fields;
			parseFileLineIntoFields(line, fields, cDefaultTableFieldsSplitter);
			for (int i = 0; i < fields.size(); i++) {
				for (int iN = 0; iN < fieldsNames.size(); iN++) {
					if (fields[i] == fieldsNames[iN]) {
						fieldsPositions[fieldsNames[iN]] = i;
						break;
					}
				}
			}
			numParsed = 0;
			for (int iN = 0; iN < fieldsNames.size(); iN++) {
				if (fieldsPositions[fieldsNames[iN]] != -1) {
					numParsed++;
				}
			}
		}
		return numParsed;
	}

	int parseFileLine(std::ifstream& infile, std::vector<std::string>& fields)
	{
		std::string line;
		int numParsed;

		if (std::getline(infile, line)) {
			numParsed = parseFileLineIntoFields(line, fields, '\t');
		}
		else {
			numParsed = -1;
		}
		return numParsed;
	}

	int parseFileLine(std::ifstream& infile, std::map<std::string, int>& fieldsPositions, std::map<std::string, std::string>& fieldsParsed)
	{
		int numParsed = -1;

		for (std::map<std::string, int>::iterator it = fieldsPositions.begin(); it != fieldsPositions.end(); ++it) {
			fieldsParsed[it->first] = std::string("");
		}

		std::string line;
		if (std::getline(infile, line)) {
			std::vector<std::string> fields;
			int numFields = parseFileLineIntoFields(line, fields, '\t');
			numParsed = 0;
			for (int iField = 0; iField < numFields; iField++) {
				for (std::map<std::string, int>::iterator it = fieldsPositions.begin(); it != fieldsPositions.end(); ++it) {
					if (it->second == iField) {
						fieldsParsed[it->first] = fields[iField];
						numParsed++;
						break;
					}
				}
			}
		}
		return numParsed;
	}

	int parseFileLineIntoFields(const std::string& s, std::vector<std::string>& tokens, char delimiter)
	{

		std::stringstream tokenStream(s);
		int nTokens = 0;
		do {
			std::string token;
			std::getline(tokenStream, token, delimiter);
			tokens.push_back(trim(token, " \r\n"));
			nTokens++;
		} while (!tokenStream.eof());
		return nTokens;
/*
std::string token;
		std::stringstream tokenStream(s);
		int nTokens = 0;
		while (std::getline(tokenStream, token, delimiter))
		{
			tokens.push_back(trim(token, " \r\n"));
			nTokens++;
		}
		return nTokens;
*/
	}

	int parseAllFileHeader(std::ifstream& infile, std::vector<std::string>& fieldsNames, std::map<std::string, int>& fieldsPositions)
	{
		int numParsed = 0;

		std::string line;
		if (std::getline(infile, line)) {
			std::vector<std::string> fields;
			parseFileLineIntoFields(line, fields, cDefaultTableFieldsSplitter);
			for (int i = 0; i < fields.size(); i++) {
				fieldsNames.push_back(fields[i]);
				fieldsPositions.insert(std::pair<std::string, int>(fields[i], i));
			}
			numParsed = fields.size();
		}
		return numParsed;
	}

	int parseAllFileHeaderTitles(std::ifstream& infile, std::vector<std::string>& fieldsNames,
		std::map<std::string, std::string>& fieldsTitles)
	{
		int numParsed = 0;

		std::string line;
		if (std::getline(infile, line)) {
			std::vector<std::string> fields;
			parseFileLineIntoFields(line, fields, cDefaultTableFieldsSplitter);
			for (int i = 0; i < fields.size(); i++) {
				fieldsTitles.insert(std::pair<std::string, std::string>(fieldsNames[i], fields[i]));
			}
			numParsed = fields.size();
		}
		return numParsed;
	}

	int parseAllFileHeaderFlags(std::ifstream& infile, std::vector<std::string>& fieldsNames,
		std::map<std::string, long int>& fieldsFlags)
	{
		int numParsed = 0;

		std::string line;
		if (std::getline(infile, line)) {
			std::vector<std::string> fields;
			parseFileLineIntoFields(line, fields, cDefaultTableFieldsSplitter);
			for (int i = 0; i < fields.size(); i++) {
				char *ptr;
				long int iFlags = strtol(fields[i].c_str(), &ptr, 16);
				fieldsFlags.insert(std::pair<std::string, long int>(fieldsNames[i], iFlags));
			}
			numParsed = fields.size();
		}
		return numParsed;
	}

	std::string toLower(std::string s) {
		for (int i = 0; i < s.size(); i++) {
			//s[i] = std::tolower(static_cast<unsigned char>(s[i]),std::locale());
			s[i] = tolower(static_cast<unsigned char>(s[i]));
		}
		return s;
	}

	bool isDigitsOnly(std::string s) {
		bool bReturn = true;
		bool bDigits = false;

		for (int i = 0; i < s.size(); i++) {
			if (s[i] != ' ' && !isdigit(s[i])) {
				bReturn = false;
				break;
			}
			if (isdigit(s[i])) {
				bDigits = true;
			}
		}
		return bReturn & bDigits;
	}


	bool isEmpty(std::string s) {
		bool bReturn = true;

		for (int i = 0; i < s.size(); i++) {
			if (s[i] != ' ' &&  s[i] != '\r' && s[i] != '\n' && s[i] != '\t') {
				bReturn = false;
				break;
			}
		}
		return bReturn;
	}

	
	int splitStringIntoVector( std::string& s, std::vector<std::string>& splitted, const char *delimiters )
    {
        int len = s.length();
        int start = 0;
        bool started = true;
        int numSplitted=0;

        int i = 0;
        for(  ; i < len ; i++ ) {
          if( !isOneOfTheFollowing( s[i], (char*)delimiters ) ) {
            start = i;
            started = true;
            break;
          }
        }
        if( !started ) {
          return -1;
        }

        for( ; i < len ; i++ ) {
          if( isOneOfTheFollowing( s[i], (char*)delimiters ) ) {
            splitted.push_back( s.substr( start, i-start ) );
            numSplitted++;
            started = false;
            for( int j=i+1 ; j < len ; j++ ) {
              if( !isOneOfTheFollowing( s[j], (char*)delimiters ) ) {
                i = j;
                start = j;
                started = true;
                break;
              }
            }
            if( !started ) {
             break;
            }
          }
        }
        if( started ) {
            splitted.push_back( s.substr( start, len-1 ) );          
        }

      return numSplitted;
    }

    bool isOneOfTheFollowing( char c, char *among ) {
      int len = strlen(among);
      for( int i = 0 ; i < len ; i++ ) {
        if( c == among[i] ) {
          return true;
        }
      }
      return false;
    }	
	

// ***************************************************************************************************************************
    // XML SECTION

    static int xmlFindStartingBracket( char *contents );
    static int xmlSubstrInStr( char *substr, char *str );


    int xmlLoad( const char *fileName, std::string& fileContents ) {
        int returnValue = -1;

        std::ifstream in( fileName, std::ifstream::binary );
        if ( in.is_open() ) {
            
            in.seekg(0, std::ios::end);
            size_t len = in.tellg();
            in.seekg(0);

            fileContents.reserve(len);
            while( !in.eof() ) {
                fileContents += in.get();
            }   
            in.close();

            returnValue = len;
        }
        return returnValue;
    }


    char *xmlLoad( const char *fileName ) {
        char *returnValue = NULL;

        std::ifstream in( fileName, std::ios::binary );
        if( in.is_open() ) {
            
            in.seekg(0, std::ios::end);
            size_t len = in.tellg();
            in.seekg(0);

            char *buffer = new char [len+1];
            if( buffer != 0x0 ) {
                in.read( buffer, len );
                buffer[len] = '\x0';
                in.close();         
                returnValue = buffer;
            }
        }
        return returnValue;
    }


    char *xmlGetNextTag( char *_contents, int &_offset ) {
        char *tag=NULL;

        if( _offset < strlen(_contents) ) {
            char *contents = &_contents[_offset];
            int startingBracket = xmlFindStartingBracket( contents );
            if( startingBracket >= 0 ) {
                //std::cout << "startingBracket=" << startingBracket << std::endl;
                int endingBracket = xmlSubstrInStr( (char*)">", &contents[startingBracket] );
                //std::cout << "endingBracket=" << endingBracket << std::endl;
                if( endingBracket > 1 ) {
                    tag = new char [ endingBracket ];
                    if( tag != 0x0 ) {
                        strncpy( tag, &contents[startingBracket+1], endingBracket-1 );
                        tag[endingBracket-1] = '\x0';

                        _offset += startingBracket + endingBracket + 1;
                    }
                }
            }
        }
        return tag;
    }


    char *xmlReadTagValue( char *tag, char *contents ) {
        char *buffer=NULL;

        int tagLen = strlen(tag);
        char *openingTag = new char [tagLen+3];
        if( openingTag != 0x0 ) {
            openingTag[0] = '<';
            openingTag[1] = '\x0';
            strcat( openingTag, tag);
            openingTag[tagLen+1] = '>'; 
            openingTag[tagLen+2] = '\x0';   
            int openingTagStart = xmlSubstrInStr( openingTag, contents ); 
            if( openingTagStart >= 0 ) {
                char *closingTag = new char [tagLen+4];
                if( closingTag != 0x0 ) {
                    closingTag[0] = '<';
                    closingTag[1] = '/';
                    closingTag[2] = '\x0';
                    strcat( closingTag, tag );
                    closingTag[tagLen+2] = '>'; 
                    closingTag[tagLen+3] = '\x0';

                    int tagContentStart = openingTagStart+tagLen+2;
                    int closingTagStart = xmlSubstrInStr( closingTag, &contents[tagContentStart] ); 
                    if( closingTagStart >= 0 ) {
                        int bufferLen = closingTagStart;
                        buffer = new char [bufferLen+1];
                        strncpy( buffer, &contents[tagContentStart], bufferLen);
                        buffer[bufferLen] = '\x0';  
                    }           
                    delete [] closingTag;
                }
            }           
            delete [] openingTag;
        }
        return buffer;
    }


    static int xmlFindStartingBracket( char *contents ) {
        int returnValue = -1;
        int startingBracket = 0, overallOffset = 0;
        int len = strlen(contents);

        while(true) {
            startingBracket = xmlSubstrInStr( (char*)"<", &contents[startingBracket] );
            if( startingBracket == -1 ) {
                break;
            }
            if( startingBracket >= len-2 ) {
                break;
            }
            if( contents[startingBracket+1] != '/' ) {
                returnValue = overallOffset + startingBracket;
                break;
            }
            startingBracket += 1;
            overallOffset += startingBracket;
        }
        return returnValue;
    }

    static int xmlSubstrInStr( char *substr, char *str ) {
        int returnValue = -1;
        int istr, isubstr;

        int strLen = strlen(str);
        int substrLen = strlen( substr );
        if( substrLen <= strLen ) {
            for( int i = 0 ; i < strLen - substrLen + 1 ; i++ ) {
                bool substringFound = true;
                for( istr=i, isubstr=0 ; isubstr < substrLen ; istr++, isubstr++ ) {
                    if( str[istr] != substr[isubstr] ) {
                        substringFound=false;
                        break;
                    }
                }
                if( substringFound ) {
                    returnValue = i;
                    break;
                }
            }
        }
        return returnValue;
    }	
	
}


