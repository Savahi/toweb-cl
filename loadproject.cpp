#include "stdafx.h"

namespace Spider3d {

    static int fieldsToReadNum = 4;
    static const char *fieldsToRead[] = { "Code", "Name", "ProjVer", "CurTime" };

	static void parseProjVer( Project &project );
	
    int loadProject( Project& project, const char *cpFile ) {
        int iReturn = -1;

        std::vector<std::string> fieldsNames;
        for( int i= 0 ; i < fieldsToReadNum ; i++ ) {
            fieldsNames.push_back( fieldsToRead[i] );
        }

        std::ifstream infile( cpFile );
        if( infile.is_open() ) {
            int numHeaderParsed = parseAllFileHeader( infile, project.fieldsNames, project.fieldsPositions );
            if( numHeaderParsed != -1 ) {
                int numFlagsParsed = parseAllFileHeaderFlags( infile, project.fieldsNames, project.fieldsFlags );
                if( numFlagsParsed == numHeaderParsed ) {
                    int numTitlesParsed = parseAllFileHeaderTitles( infile, project.fieldsNames, project.fieldsTitles );
                    if( numTitlesParsed == numHeaderParsed ) {
                        std::map<std::string,std::string> fieldsParsed;    
                        int numParsed = parseFileLine( infile, project.fieldsPositions, fieldsParsed );
                        if( numParsed != -1 ) {
                            project.sCode = fieldsParsed["Code"];
                            project.sName = fieldsParsed["Name"];
                            project.sProjVer = fieldsParsed["ProjVer"];
							parseProjVer(project);
                            project.sCurTime = fieldsParsed["CurTime"];
                            project.sNotes = fieldsParsed["Notes"];
                            iReturn = 0;
                        }
                    }
                }
            }
            infile.close();
        }
        return iReturn;
    }

int loadProjectXML( Project& project, const char *cpFile ) {
        int iReturn = -1;

        char *xml = xmlLoad( cpFile );

        if( xml != NULL ) 
        {
            char *code = xmlReadTagValue( (char*)"Code", xml );
            if( code != NULL ) {
                // std::cout << "code=" << code << std::endl;
                project.sCode = code;
                delete [] code;
            } else {
                project.sCode = "";                
            }
            char *name = xmlReadTagValue( (char*)"Name", xml );
            if( name != NULL ) {
                //std::cout << "code=" << name << std::endl;
                project.sName = name;
                delete [] name;
            } else {
                project.sName = "";                
            }
            char *curTime = xmlReadTagValue( (char*)"CurTime", xml );
            if( curTime != NULL ) {
                //std::cout << "code=" << curTime << std::endl;
                project.sCurTime = curTime;
                delete [] curTime;
            } else {
                project.sCurTime = "";                
            }
            char *version = xmlReadTagValue( (char*)"ProjVer", xml );
            if( version != NULL ) {
                project.sProjVer = version;
                //std::cout << "code=" << version << std::endl;
                delete [] version;
            } else {
                project.sProjVer = "0.0";
            }
            parseProjVer(project);

            char *start = xmlReadTagValue( (char*)"Start", xml );
            if( start != NULL ) {
                //std::cout << "code=" << name << std::endl;
                project.sStart = start;
                delete [] start;
            } else {
                project.sStart = "";                
            }
            char *finish = xmlReadTagValue( (char*)"Finish", xml );
            if( finish != NULL ) {
                //std::cout << "code=" << name << std::endl;
                project.sFinish = finish;
                delete [] finish;
            } else {
                project.sFinish = "";                
            }
            char *notes = xmlReadTagValue( (char*)"Notes", xml );
            if( notes != NULL ) {
                //std::cout << "code=" << name << std::endl;
                project.sNotes = notes;
                delete [] notes;
            } else {
                project.sNotes = "";                
            }

            char *calendars = xmlReadTagValue( (char*)"Calendars", xml );
            if( calendars != NULL ) {
                int offset = 0;

                for( int i = 0 ; ; i++ ) {
                    char *tag = xmlGetNextTag( calendars, offset );
                    if( tag == NULL ) {
                        break;
                    }
                    char *calendar = xmlReadTagValue( tag, calendars );
                    if( calendar != NULL ) {
                        std::vector<std::string> dates;
                        std::string calendarStr(calendar);
                        int numDates = splitStringIntoVector( calendarStr, dates, " \r\n" );
                        if( numDates > 0 && numDates % 2 == 0 ) {
                            project.calendars.insert( std::pair< std::string, std::vector<std::string> >( std::string(tag), dates ) );
                        }
                    }
                    //std::cout << "tag=" << tag << " (offset=" << offset << ")" << std::endl;
                    //std::cout << "calendar=" << calendar << std::endl;
                    delete [] calendar;
                    delete [] tag;
                }
                delete [] calendars;
            }
            delete [] xml;
            iReturn=0;
        } 

        return iReturn;
    }
	
    static void parseProjVer( Project &project ) {
        int nScanned = sscanf( project.sProjVer.c_str(), "%d", &project.iProjVer );
        if( nScanned == 1 ) {
            project.bProjVer = true;
        } else {
            project.iProjVer = 0;                    
            project.bProjVer = false;
        }    
    }
}