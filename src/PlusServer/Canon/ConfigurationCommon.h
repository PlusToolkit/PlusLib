/**
***************************************************************************************************
* @file      ConfigurationCommon.h
*
* @brief     Defines the ConfigurationCommon class and the implementations of its inline methods.
*
* @copyright Canon Medical Research USA, Inc. all rights reserved. No
*            reproduction of any kind shall be made without prior written
*            consent of Canon Medical Research USA, Inc.
*
***************************************************************************************************
*/
#ifndef __CONFIGURATION_COMMON_H__
#define __CONFIGURATION_COMMON_H__

#include <memory>
#include <string>
#include <map>
#include <type_traits>
#include <cassert>
#include <exception>
#include "cxxopts.hpp"

/**
 * @brief This class encapsulates the processing of configuration options common to several
 * applications.
 *
 * It currently supports only command line options, but should be extensible to a configuration file
 * and/or environment variable methods as well.
 *
 * To use it, simply instantiate a ConfigurationCommon object where you need one. To process command
 * line options, simply call the load method passing the argc and argv arguments passed into the
 * application's main function.
 *
 * Call getValue with one of the ConfigurationCommon::Option enumerations to obtain the associated
 * configuration setting.
 *
 * It is an error to call a get* method prior to calling the load method.
 *
 * Application specific options can be added by defining a Configuration class that inherits from
 * ConfigurationCommon. The dervied class should have its own Option enum class and mOptionString
 * map that define that allowed options and the string used to specify them.  The dervied class
 * should also overload the getValue method taking a parameter of the derived class Option enum.
 * It can then be implemented using the protected getValue method that takes a string parameter.
 */
class ConfigurationCommon
{
  public:
    enum class Option
    {
        HELP,
        NCSA_DOMAIN,
        AUTHORITY,
        OPTOPIC,
        DATASRCTOPIC,
        QOSLIBRARY,
        QOSPROFILE,
        IGNORESELF,
        SUBNETS,
        // Pseudo options that are constructed from real options.
        COMPOSITION_SPACE,
    };

    //! The default constructor.
    ConfigurationCommon() = default;

    //! A constructor that will set the application description.
    ConfigurationCommon(const std::string& appDesc) { mAppDesc = appDesc; }

    //! A virtual default destructor.
    virtual ~ConfigurationCommon() = default;

    ///@name Deleted methods
    ///@{
    ConfigurationCommon(const ConfigurationCommon&) = delete;
    ConfigurationCommon& operator=(const ConfigurationCommon&) = delete;
    ConfigurationCommon(ConfigurationCommon&&) = delete;
    ConfigurationCommon& operator=(ConfigurationCommon&&) = delete;
    ///@}

    /**
     * @brief Loads the configuration by parsing the specified command line.
     *
     * @param [in] argc The number of arguments passed to the program.
     * @param [in] argv Pointer to an array of argc+1 pointers containing the program's arguments.
     */
    void load(int argc, char* argv[]);

    /**
     * @brief Returns the entire command line that invoked the application.
     *
     * @return The entire command line that invoked the application.
     */
    const std::string& getCommandLine() const;

    /**
     * @brief Returns the configuration option specified in the parameter.
     *
     * @param [in] The enumeration of the desired configuration option.
     *
     * @return The value of the specified configuration option.
     */
    template<typename R>
    R getValue(ConfigurationCommon::Option option) const;

  protected:
    /**
     * @brief Returns the configuration option specified in the parameter.
     *
     * @param [in] A string representing the desired configuration option.
     *
     * @return The value of the specified configuration option.
     *
     * @note This method is intended to aid in the implementation of an overloaded getValue method
     * in a dervied class that takes a parameter of the derived class' Option enum.
     */
    template<typename R>
    R getValue(const std::string& option) const;

  private:
    /**
     * @brief Allows a dervied class to provide an application description.
     *
     * @return A reference to an application description string.
     */
    virtual const std::string& getAppDesc() const;

    /**
     * @brief Allows a dervied class to add application specific options.
     *
     * @param [in] A reference to the object that represents the application command line options.
     */
    virtual void addOptions(cxxopts::Options&) {}

    /**
     * @brief Returns a string intended to be passed to the config parameter of the
     * ncsa::CompositionSpace::join method.
     *
     * @return A string to be passed as the config parameter to ncsa::CompositionSpace::join.
     */
    std::string getCompositionSpaceConfig() const;

    //! Unique pointer to the results of parsing the command line.
    static std::unique_ptr<cxxopts::ParseResult> mConfig;

    //! String that contains a description of the application.
    static std::string mAppDesc;

    //! Unique pointer to the concatenated command line arguments.
    static std::string mCommandLine;

    //! Map of Option enumerators to the associated option string used on the command line.
    static std::map<Option, std::string> mOptionString;
};

template<typename R>
R ConfigurationCommon::getValue(ConfigurationCommon::Option option) const
{
    assert(mConfig);

    switch (option)
    {
        case ConfigurationCommon::Option::COMPOSITION_SPACE:
            if constexpr (std::is_same_v<R, std::string>)
            {
                return getCompositionSpaceConfig();
            }
            else
            {
                return R();
            }
            break;

        default:
            try
            {
                return (*mConfig)[mOptionString[option]].as<R>();
            }
            catch (const std::domain_error&)
            {
                return R();
            }
            break;
    }
}

template<typename R>
R ConfigurationCommon::getValue(const std::string& option) const
{
    assert(mConfig);

    try
    {
        return (*mConfig)[option].as<R>();
    }
    catch (const std::domain_error&)
    {
        return R();
    }
}
#endif
