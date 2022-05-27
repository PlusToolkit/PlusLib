/**
***************************************************************************************************
* @file      ConfigurationCommon.cpp
*
* @brief     Defines the implementation of non-inline methods of the ConfigurationCommon class.
*
* @copyright Canon Medical Research USA, Inc. all rights reserved. No
*            reproduction of any kind shall be made without prior written
*            consent of Canon Medical Research USA, Inc.
*
***************************************************************************************************
*/

#include <filesystem>
#include <iostream>
#include "ConfigurationCommon.h"

std::unique_ptr<cxxopts::ParseResult> ConfigurationCommon::mConfig{ nullptr };
std::string ConfigurationCommon::mAppDesc{ "\nNCSA Application.\n" };
std::string ConfigurationCommon::mCommandLine;
std::map<ConfigurationCommon::Option, std::string> ConfigurationCommon::mOptionString = {
    { ConfigurationCommon::Option::HELP, std::string("help") },
    { ConfigurationCommon::Option::NCSA_DOMAIN, std::string("domain") },
    { ConfigurationCommon::Option::AUTHORITY, std::string("authority") },
    { ConfigurationCommon::Option::OPTOPIC, std::string("optopic") },
    { ConfigurationCommon::Option::DATASRCTOPIC, std::string("datasrctopic") },
    { ConfigurationCommon::Option::QOSLIBRARY, std::string("qoslibrary") },
    { ConfigurationCommon::Option::QOSPROFILE, std::string("qosprofile") },
    { ConfigurationCommon::Option::IGNORESELF, std::string("ignoreself") },
    { ConfigurationCommon::Option::SUBNETS, std::string("subnets") },
};

void ConfigurationCommon::load(int argc, char* argv[])
{
    std::filesystem::path application{ argv[0] };
    auto appName = application.stem().string();

    cxxopts::Options options(appName, getAppDesc());

    // clang-format off
    options
        .allow_unrecognised_options()
        .add_options()
        ("h,help", "Print help")
    ;

    options
        .add_options("CommLib")
        (mOptionString[Option::NCSA_DOMAIN], "NCSA domain (0-230)",
            cxxopts::value<std::string>(), "N")
        (mOptionString[Option::AUTHORITY], "NCSA authority",
            cxxopts::value<std::string>(), "hostname")
        (mOptionString[Option::OPTOPIC], "NCSA Operator topic",
            cxxopts::value<std::string>(), "string")
        (mOptionString[Option::DATASRCTOPIC], "NCSA DataSource topic",
            cxxopts::value<std::string>(), "string")
        (mOptionString[Option::QOSLIBRARY], "NCSA QoS library",
            cxxopts::value<std::string>(), "string")
        (mOptionString[Option::QOSPROFILE], "NCSA QoS profile",
            cxxopts::value<std::string>(), "string")
        (mOptionString[Option::IGNORESELF], "Don't receive what we publish",
            cxxopts::value<bool>()->default_value("false"))
        (mOptionString[Option::SUBNETS], "Subnets to include",
            cxxopts::value<std::string>(), "string")
    ;
    // clang-format on

    // Add application specific options.
    addOptions(options);

    mCommandLine.clear();
    for (auto arg = 0; arg < argc; ++arg)
    {
        mCommandLine += argv[arg];
        mCommandLine += " ";
    }

    mConfig = std::make_unique<cxxopts::ParseResult>(options.parse(argc, argv));

    if (mConfig->count(mOptionString[Option::HELP]) != 0)
    {
        std::cout << options.help({ "", "CommLib" }) << std::endl;
        exit(0);
    }
}

const std::string& ConfigurationCommon::getAppDesc() const
{
    return mAppDesc;
}

const std::string& ConfigurationCommon::getCommandLine() const
{
    return mCommandLine;
}

std::string ConfigurationCommon::getCompositionSpaceConfig() const
{
    std::string config{};

    for (auto optionName : { Option::NCSA_DOMAIN,
                             Option::AUTHORITY,
                             Option::OPTOPIC,
                             Option::DATASRCTOPIC,
                             Option::QOSLIBRARY,
                             Option::QOSPROFILE,
                             Option::SUBNETS })
    {
        if (auto value = getValue<std::string>(optionName); !value.empty())
        {
            config += ',' + mOptionString[optionName] + '=' + value;
        }
    }

    // The compositor default is opposite of the commlib default.
    if (!getValue<bool>(Option::IGNORESELF))
    {
        config += ',' + mOptionString[Option::IGNORESELF] + "=false";
    }

    // Remove any leading comma character.
    if (!config.empty())
    {
        config = config.substr(1u);
    }

    return config;
}
