#include <sstream>
#include "GlobalFuncs.h"

void splitStr(std::vector<std::string>& dest, const std::string& str, const char* separator)
{
	char* pTempStr = strdup( str.c_str() );
	char* pWord = std::strtok(pTempStr, separator);
	while(pWord != NULL)
	{
		dest.push_back(pWord);
		pWord = std::strtok(NULL, separator);
	}
	free(pTempStr);
}

void replaceSubStr(std::string& str, const std::string& from, const std::string& to)
{
	if(from.empty())
	return;
	size_t start_pos = 0;
	while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
		str.replace(start_pos, from.length(), to);
		start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
	}
}

void RemoveSubStr(std::string& sInput, const std::string& sub)
{
	std::string::size_type foundpos = sInput.find(sub);
	if ( foundpos != std::string::npos )
	sInput.erase(sInput.begin() + foundpos, sInput.begin() + foundpos + sub.length());
}

std::vector<std::string> splitCmdArgs( std::string str)
{
	std::vector<std::string> args;
	std::string arg;
	arg.clear();
	bool quote_opened=false;
	for(int i=0; i<str.size();i++)
	{
		if(quote_opened)
			goto qstr;
		if(str[i] == ' ')
		{
			if(!arg.empty())
				args.push_back(arg);
			arg.clear();
			continue;
		}
		if(str[i] == '\"')
		{
			quote_opened=true;
			continue;
		}
		arg.push_back(str[i]);
	continue;

		qstr:
			if(str[i] == '\"')
			{
				if(!arg.empty())
					args.push_back(arg);
				arg.clear();
				quote_opened=false;
				continue;
			}
			arg.push_back(str[i]);
	}

	if(!arg.empty())
				args.push_back(arg);

	return args;
}


std::wstring utf8_decode(const std::string &str)
{
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo( size_needed, 0 );
    MultiByteToWideChar                  (CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string utf8_encode(const std::wstring &wstr)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo( size_needed, 0 );
    WideCharToMultiByte                  (CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::string wstr2str(const std::wstring &wstr)
{
	std::wstring ws = wstr;
	std::string s;
    const std::locale locale("");
    typedef std::codecvt<wchar_t, char, std::mbstate_t> converter_type;
    const converter_type& converter = std::use_facet<converter_type>(locale);
    std::vector<char> to(ws.length() * converter.max_length());
    std::mbstate_t state;
    const wchar_t* from_next;
    char* to_next;
    const converter_type::result result = converter.out(state,
		wstr.data(), wstr.data() + wstr.length(),
		from_next, &to[0], &to[0] + to.size(), to_next);
    if (result == converter_type::ok || result == converter_type::noconv)
	{
      s = std::string(&to[0], to_next);
    }
	return s;
}

std::string i2str(int source)
{
	std::stringstream s;
	s<<source;
	return s.str();
}


bool file_existsX(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}
