#include "PlusConfigure.h"
#include "vtksys/CommandLineArguments.hxx"
#include "PlusMath.h"
#include "vtkXMLDataElement.h"
#include "vtkXMLUtilities.h"

const double DOUBLE_DIFF = 0.0001; // used for comparing double numbers


// ************************ PlusMath::LSQRMinimize ****************************
int TestLSQRMinimize(vtkXMLDataElement* xmlPlusMathTest); 
PlusStatus ReadLSQRDataFromXml(vtkXMLDataElement* xmlLSQRMinimize, std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector); 
PlusStatus GenerateLSQRData(vtkXMLDataElement* xmlLSQRMinimize, int numberOfData, int numberOfOutliers); 

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int numberOfErrors(0); 

  bool printHelp(false);

  int verboseLevel = vtkPlusLogger::LOG_LEVEL_INFO;

  vtksys::CommandLineArguments args;
  args.Initialize(argc, argv);
  std::string inputDataFileName(""); 

  args.AddArgument("--help", vtksys::CommandLineArguments::NO_ARGUMENT, &printHelp, "Print this help.");	
  args.AddArgument("--verbose", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &verboseLevel, "Verbose level (1=error only, 2=warning, 3=info, 4=debug)");	
  args.AddArgument("--input-data-file", vtksys::CommandLineArguments::EQUAL_ARGUMENT, &inputDataFileName, "Input XML data file name");	

  if ( !args.Parse() )
  {
    std::cerr << "Problem parsing arguments" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  if ( printHelp ) 
  {
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_SUCCESS); 
  }

  if ( inputDataFileName.empty() )
  {
    std::cerr << "input-data-file argument required!" << std::endl;
    std::cout << "Help: " << args.GetHelp() << std::endl;
    exit(EXIT_FAILURE);
  }

  vtkPlusLogger::Instance()->SetLogLevel(verboseLevel);
  vtkPlusLogger::Instance()->SetDisplayLogLevel(verboseLevel);

  vtkSmartPointer<vtkXMLDataElement> xmlPlusMathTest = vtkXMLUtilities::ReadElementFromFile(inputDataFileName.c_str()); 

  if ( xmlPlusMathTest == NULL )
  {
    LOG_ERROR("Failed to read input xml configuration file from file: " << inputDataFileName );
    exit(EXIT_FAILURE);
  }
  
  // Test PlusMath::LSQRMinimize 
  numberOfErrors += TestLSQRMinimize(xmlPlusMathTest); 

  if ( numberOfErrors > 0 ) 
  {
    LOG_INFO("Test failed!"); 
    return EXIT_FAILURE; 
  }

  //vtkXMLUtilities::WriteElementToFile(xmlPlusMathTest, "d:/PlusMathTestData.xml", &vtkIndent(2)); 

  LOG_INFO("Test finished successfully!"); 
  return EXIT_SUCCESS; 
}

//----------------------------------------------------------------------------
int TestLSQRMinimize(vtkXMLDataElement* xmlPlusMathTest)
{
  LOG_INFO("Testing PlusMath::LSQRMinimize function..."); 

  int numberOfErrors(0); 

  if ( xmlPlusMathTest == NULL )
  {
    LOG_ERROR("Unable to test LSQRMinimize - input xmlPlusMathTest xmld data element is NULL!"); 
    numberOfErrors++; 
    return numberOfErrors; 
  }

  std::vector<vnl_vector<double>> aMatrix; 
  std::vector<double> bVector;  
  vnl_vector<double> resultVector(2,0); 

  vtkSmartPointer<vtkXMLDataElement> xmlLSQRMinimize = xmlPlusMathTest->FindNestedElementWithName("LSQRMinimize"); 

  if ( xmlLSQRMinimize != NULL )
  {
    //GenerateLSQRData(xmlLSQRMinimize, 10000, 1000); 

    ReadLSQRDataFromXml(xmlLSQRMinimize, aMatrix, bVector); 

    if ( PlusMath::LSQRMinimize(aMatrix, bVector, resultVector) != PLUS_SUCCESS )
    {
      LOG_ERROR("Failed to run LSQRMinimize on dataset!"); 
      numberOfErrors++; 
    }

    LOG_INFO("Linear equation: y = " << resultVector[1] << " + " << resultVector[0] << " x "); 

    // Compare result to baseline
    vtkSmartPointer<vtkXMLDataElement> xmlResult = xmlLSQRMinimize->FindNestedElementWithName("Result"); 
    if ( xmlResult != NULL )
    {
      double x0Base(0); 
      if ( !xmlResult->GetScalarAttribute("x0", x0Base) )
      {
        LOG_ERROR("Unable to find x0 attribute under LSQRMinimize Result tag!"); 
        numberOfErrors++; 
      }
      else
      {
        if ( fabs(x0Base - resultVector[1]) > DOUBLE_DIFF )
        {
          LOG_ERROR("x0 differ from baseline (current: " << std::fixed << resultVector[1] << "  baseline: " << x0Base << ")!"); 
          numberOfErrors++; 
        }
      }

      double xBase(0); 
      if ( !xmlResult->GetScalarAttribute("x", xBase) )
      {
        LOG_ERROR("Unable to find x attribute under LSQRMinimize Result tag!"); 
        numberOfErrors++; 
      }
      else
      {
        if ( fabs(xBase - resultVector[0]) > DOUBLE_DIFF )
        {
          LOG_ERROR("x differ from baseline (current: " << std::fixed << resultVector[0] << "  baseline: " << xBase << ")!"); 
          numberOfErrors++; 
        }
      }

    }
    else
    {
      LOG_ERROR("Unable to find Result xml data element under LSQRMinimize!"); 
      numberOfErrors++; 
    }

  }
  else
  {
    LOG_ERROR("Unable to find LSQRMinimize xml data element in config file!"); 
    numberOfErrors++; 
  }

  return numberOfErrors; 
}

//----------------------------------------------------------------------------
PlusStatus ReadLSQRDataFromXml(vtkXMLDataElement* xmlLSQRMinimize, std::vector<vnl_vector<double>> &aMatrix, std::vector<double> &bVector)
{
  vtkSmartPointer<vtkXMLDataElement> xmlLinearEquation = xmlLSQRMinimize->FindNestedElementWithName("LinearEquation"); 
  if ( xmlLinearEquation == NULL )
  {
    LOG_ERROR("Unable to find LinearEquation xml data element in file!"); 
    return PLUS_FAIL; 
  }

  int numberOfUnknowns(0); 
  if ( !xmlLinearEquation->GetScalarAttribute("NumberOfUnknowns", numberOfUnknowns) )
  {
    LOG_ERROR("Unable to find NumberOfUnknowns attribute!"); 
    return PLUS_FAIL; 
  }

  int numberOfEquations(0); 
  if ( !xmlLinearEquation->GetScalarAttribute("NumberOfEquations", numberOfEquations) )
  {
    LOG_ERROR("Unable to find NumberOfEquations attribute!"); 
    return PLUS_FAIL; 
  }

  double* aMatrixArray = new double[numberOfEquations*numberOfUnknowns]; 
  if ( !xmlLinearEquation->GetVectorAttribute("aMatrix", numberOfEquations*numberOfUnknowns, aMatrixArray) )
  {
    LOG_ERROR("Unable to find aMatrix attribute!"); 
    return PLUS_FAIL; 
  }

  double* bVectorArray = new double[numberOfEquations]; 
  if ( !xmlLinearEquation->GetVectorAttribute("bVector", numberOfEquations, bVectorArray) )
  {
    LOG_ERROR("Unable to find bVector attribute!"); 
    return PLUS_FAIL; 
  }

  aMatrix.clear(); 
  vnl_vector<double> rowVector(numberOfUnknowns); 
  for ( int row = 0; row < numberOfEquations*numberOfUnknowns; row = row + numberOfUnknowns)
  {
    for ( int i = 0; i < numberOfUnknowns; ++i )
    {
      rowVector[i] = aMatrixArray[row + i]; 
    }
    aMatrix.push_back(rowVector); 
  }
  delete[] aMatrixArray; 

  bVector.clear(); 
  for ( int row = 0; row < numberOfEquations; ++row)
  {
    bVector.push_back(bVectorArray[row]); 
  }
  delete[] bVectorArray; 

  return PLUS_SUCCESS; 
}

//----------------------------------------------------------------------------
PlusStatus GenerateLSQRData(vtkXMLDataElement* xmlLSQRMinimize, int numberOfData, int numberOfOutliers)
{
  // Generate y = x - 1 linear equation coefficients for LSQR optimizer test and save it to xmlData 
  // Add some outliers also to the dataset 

  if ( xmlLSQRMinimize == NULL )
  {
    LOG_ERROR("Failed to generate LSQR data into xml data - xmlLSQRMinimize is NULL!"); 
    return PLUS_FAIL;
  }

  const int numberOfUnknowns(2);  
  std::vector<double> aMatrix; 
  std::vector<double> bVector;  

  for ( int i = 0; i < numberOfData; ++i )
  {
    double sign = (i%2==0?1:-1); 
    double m = 100 * double(rand())/double(RAND_MAX);
    double y = ( m - 1 ) + sign*double(rand())/double(RAND_MAX); 

    aMatrix.push_back(m); 
    aMatrix.push_back(1); 
    bVector.push_back(y); 
  }

  // Outliers
  for ( int i = 0; i < numberOfOutliers; ++i )
  {
    double sign = (i%2==0?1:-1); 
    double m = 100 * double(rand())/double(RAND_MAX);
    double y = ( m - 1 ) + sign*double(rand())/double(RAND_MAX); 

    m = m + sign*m*(0.5); 
    y = y - sign*y*(0.5); 

    aMatrix.push_back(m); 
    aMatrix.push_back(1); 
    bVector.push_back(y); 
  }

  vtkSmartPointer<vtkXMLDataElement> xmlLinearEquation = xmlLSQRMinimize->FindNestedElementWithName("LinearEquation"); 
  if ( xmlLinearEquation == NULL )
  {
    xmlLinearEquation = vtkSmartPointer<vtkXMLDataElement>::New(); 
    xmlLinearEquation->SetName("LinearEquation"); 
    xmlLinearEquation->SetParent(xmlLSQRMinimize); 
    xmlLSQRMinimize->AddNestedElement(xmlLinearEquation); 
  }

  xmlLinearEquation->SetIntAttribute("NumberOfUnknowns", numberOfUnknowns); 
  xmlLinearEquation->SetIntAttribute("NumberOfEquations", bVector.size()); 
  xmlLinearEquation->SetVectorAttribute("aMatrix", aMatrix.size(), &aMatrix[0]); 
  xmlLinearEquation->SetVectorAttribute("bVector", bVector.size(), &bVector[0]); 

  return PLUS_SUCCESS; 

}
