# Citation

If you use PlusLib in your research or publications, please cite the following paper:

## Primary Citation

> Andras Lasso, Tamas Heffter, Adam Rankin, Csaba Pinter, Tamas Ungi, and Gabor Fichtinger, "PLUS: Open-source toolkit for ultrasound-guided intervention systems", **IEEE Transactions on Biomedical Engineering**, vol. 61, no. 10, pp. 2527-2537, Oct. 2014.  
> DOI: [10.1109/TBME.2014.2322864](https://doi.org/10.1109/TBME.2014.2322864)  
> PMID: [24833412](https://pubmed.ncbi.nlm.nih.gov/24833412/)

## BibTeX Entry

```bibtex
@ARTICLE{Lasso2014a,
  author = {Lasso, Andras and Heffter, Tamas and Rankin, Adam and Pinter, Csaba and Ungi, Tamas and Fichtinger, Gabor},
  title = {{PLUS}: Open-Source Toolkit for Ultrasound-Guided Intervention Systems},
  journal = {IEEE Transactions on Biomedical Engineering},
  year = {2014},
  month = {Oct},
  volume = {61},
  number = {10},
  pages = {2527--2537},
  doi = {10.1109/TBME.2014.2322864},
  pmid = {24833412},
  url = {http://perk.cs.queensu.ca/contents/plus-open-source-toolkit-ultrasound-guided-intervention-systems}
}
```

## Abstract

> **Purpose**: Image-guided interventions require integrated hardware and software solutions. Commercially available systems are expensive, closed-source, and difficult to extend or customize for research applications. We describe the software architecture of an open-source toolkit called PLUS (Public software Library for UltraSound imaging research) to facilitate rapid prototyping of image-guided intervention systems using tracked ultrasound imaging.
>
> **Methods**: PLUS integrates a variety of hardware devices, such as imaging devices, tracking systems, and other sensors used in image-guided interventions. The software provides functions for spatial and temporal calibration, volume reconstruction, and data streaming and recording. PLUS is implemented in C++, using VTK and Qt, and includes command-line tools and an application with a graphical user interface. The software runs on Windows and Linux platforms.
>
> **Results**: Numerous research groups have successfully applied PLUS in diverse projects, including tracked ultrasound systems, surgical navigation systems, electromagnetic tracker integration, virtual reality training simulators, and others. Several research institutions are now using PLUS for research and education.
>
> **Conclusions**: PLUS provides a robust and flexible software infrastructure for developing image-guided intervention systems. The toolkit is publicly available under a BSD-style open-source license.

## Related Publications

### Calibration

- Pace, D.F., et al. (2009). "An open-source real-time ultrasound reconstruction system for four-dimensional imaging of moving organs." *Medical Imaging 2009: Ultrasonic Imaging and Signal Processing*.

- Carbajal, G., et al. (2013). "Improving N-wire phantom-based freehand ultrasound calibration." *International Journal of Computer Assisted Radiology and Surgery*, 8(6), 1063-1072.

### Volume Reconstruction

- Lasso, A., et al. (2012). "Real-time compounding of volumetric ultrasound data." *SPIE Medical Imaging*, 8316.

### Applications

- Ungi, T., et al. (2012). "Navigated breast tumor excision using electromagnetically tracked ultrasound and surgical instruments." *IEEE Transactions on Biomedical Engineering*, 59(10), 2793-2799.

- Jolley, M.A., et al. (2017). "A computer-aided diagnosis system for evaluating patterns of ventricular volume change from cardiac ultrasound." *Circulation: Cardiovascular Imaging*, 10(8), e006083.

## Acknowledgments

Development of PlusLib has been supported by:

- Cancer Care Ontario
- Natural Sciences and Engineering Research Council of Canada
- Canadian Institutes of Health Research
- Ontario Research Fund
- National Institutes of Health (USA)
- Various equipment donations from industry partners

## Laboratory

PlusLib is developed and maintained by the **Laboratory for Percutaneous Surgery (PerkLab)** at Queen's University, Kingston, Ontario, Canada.

- **Website**: [http://perk.cs.queensu.ca](http://perk.cs.queensu.ca)
- **PlusToolkit**: [http://www.plustoolkit.org](http://www.plustoolkit.org)

## Contributing

If you have used PlusLib in your published work, please let us know! We maintain a list of publications that have used PlusLib.

Contact: [plus-users mailing list](https://github.com/PlusToolkit/PlusLib/discussions)

## See Also

- [License](license.md) - Licensing information
- [Contributing](../developer-guide/contributing.md) - How to contribute to PlusLib
