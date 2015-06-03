MODEL_CATALOG_START()

MODEL_TABLE_START("Tracking fixtures" "See below a list of fixtures that can be used for mounting tracker markers (both optical and electromagnetic) on various tools and objects.")
MODEL_TABLE_ROW(
  ID "SensorHolder_Wing_1.0"
  DESCRIPTION "Clip to mount a MarkerHolder or 8mm Ascension EM sensor to an object. With a wing to make it easier to fix it by glue or screws."
  )
MODEL_TABLE_ROW(
  ID "MarkerHolder_120mm-even_long"
  DESCRIPTION "Holder for visible-light printed black&white optical tracker markers (such as MicronTracker)."
  PRINTABLE_FILES "TrackingFixtures/MarkerHolder_120mm-even_long.stl" "TrackingFixtures/Marker_01-04.pdf"
  )
MODEL_TABLE_ROW(
  ID "MarkerHolder_120mm-odd_long"
  DESCRIPTION "Holder for visible-light printed black&white optical tracker markers (such as MicronTracker)."
  PRINTABLE_FILES "TrackingFixtures/MarkerHolder_120mm-odd_long.stl" "TrackingFixtures/Marker_01-04.pdf"
  )
MODEL_TABLE_ROW(
  ID "MarkerHolder_120mm-even_short"
  DESCRIPTION "Holder for visible-light printed black&white optical tracker markers (such as MicronTracker)."
  PRINTABLE_FILES "TrackingFixtures/MarkerHolder_120mm-even_short.stl" "TrackingFixtures/Marker_01-04.pdf"
  )
MODEL_TABLE_ROW(
  ID "MarkerHolder_120mm-odd_short"
  DESCRIPTION "Holder for visible-light printed black&white optical tracker markers (such as MicronTracker)."
  PRINTABLE_FILES "TrackingFixtures/MarkerHolder_120mm-odd_short.stl" "TrackingFixtures/Marker_01-04.pdf"
  )
MODEL_TABLE_ROW(
  ID "CauteryHolder_1.0"
  DESCRIPTION "Clamps to a tubular shaped tool, e.g. electrocauterizer. For clamp tightening use hex-head cap screw, M6 thread, 30 mm long with a matching wing nut. For assembly with SensorHolder-Ordered-HolesInterface use hex-head cap screw M4 thread, with any matching nut."
  )
MODEL_TABLE_ROW(
  ID "NeedleClip-Assembly_1.5mm_2.0"
  DESCRIPTION "Clamps to a needle of 1.5 mm diameter through a sterile bag."
  )
MODEL_TABLE_ROW(
  ID "SensorHolder-Ordered_2mm_1.0"
  DESCRIPTION "Holds either a Model 800 Ascension EM sensor, or another PLUS fixture, e.g. for holding MicronTracker markers. This part is frequently part of an assembly, but can also be used by itself."
  )
MODEL_TABLE_ROW(
  ID "SensorHolder_1.0"
  DESCRIPTION "New sensor holder design. This will replace SensorHolder-Ordered_2mm_1.0 eventually. Holds either a Model 800 Ascension EM sensor, or another PLUS fixture, e.g. for holding MicronTracker markers. This part is frequently part of an assembly, but can also be used by itself."
  )
MODEL_TABLE_ROW(
  ID "OrientationsLR_1.0"
  DESCRIPTION "This is the most simple reference sensor holder to be used on patients. In a certain surgical setting (e.g. when stuck on the chest) this defines the patient orientation. This allows saving virtual camera positions."
  )
# Add remaining experimental tools
FOREACH(MODELFILE MarkerHolder_120mm_Winged_1.0 MarkerHolder_120mm-Short_2.0 NeedleGrabberFlappy-Assembly_1.0 Plug-L_60mm_3.0 SensorHolder-GlueHoles-Ordered_2mm_1.0 SensorHolder-Ordered-HolesInterface_2mm_1.0)
  MODEL_TABLE_ROW(ID ${MODELFILE} DESCRIPTION "Experimental") 
ENDFOREACH()
MODEL_TABLE_END()

MODEL_TABLE_START("Tools" "See below a list of tools for tracking, calibration, and simulation.")
MODEL_TABLE_ROW(
  ID "Stylus_60mm"
  DESCRIPTION "Pointer tool with built-in sensor holder. 60mm long, sharp tip."
  )
MODEL_TABLE_ROW(
  ID "Stylus_100mm"
  DESCRIPTION "Pointer tool with built-in sensor holder. 100mm long, sharp tip."
  )
MODEL_TABLE_ROW(
  ID "Stylus_Candycane_100mm_WithHolder"
  DESCRIPTION "Pointer tool with built-in sensor holder. 100mm long, curved tip for ultrasound calibration."
  )  
# Add remaining experimental tools
FOREACH(MODELFILE Stylus_Candycane_70mm_1.0)
  MODEL_TABLE_ROW(ID ${MODELFILE} DESCRIPTION "Experimental") 
ENDFOREACH()
MODEL_TABLE_ROW(
  ID "SPL40-1.0"
  PRINTABLE_FILES "SimProbeLinear/SPL40-1.0.stl"  
  EDIT_LINK "${CATALOG_URL}/SimProbeLinear"
  DESCRIPTION "Simulated 40mm wide linear ultrasound probe."
  )
MODEL_TABLE_END()

MODEL_TABLE_START("Calibration phantoms" "See below a list of ultrasound calibration phantoms.")
MODEL_TABLE_ROW(
  ID "fCal-2.0"
  IMAGE_FILE "fCalPhantom/fCal_2/PhantomDefinition_fCal_2.0_Wiring_2.0.png"
  PRINTABLE_FILES "fCalPhantom/fCal_2/fCal_2.0.stl"
  EDIT_LINK "${CATALOG_URL}/fCalPhantom/fCal_2"
  DESCRIPTION "Phantom for freehand spatial ultrasound calibration for shallow depth (up to 9 cm)."
  )
MODEL_TABLE_ROW(
  ID "fCal-2.1"
  PRINTABLE_FILES "fCalPhantom/fCal_2/fCal_2.1.stl"
  EDIT_LINK "${CATALOG_URL}/fCalPhantom/fCal_2"
  DESCRIPTION "Phantom for freehand spatial ultrasound calibration for shallow depth (up to 9 cm)."
  )
MODEL_TABLE_ROW(
  ID "fCal-3.1"
  IMAGE_FILE "fCalPhantom/fCal_3/fCal3.1.png"
  PRINTABLE_FILES
    "fCalPhantom/fCal_3/fCal_3.1.stl"
    "fCalPhantom/fCal_3/fCal_3.1_back.stl"
    "fCalPhantom/fCal_3/fCal_3.1_front.stl"
    "fCalPhantom/fCal_3/fCal_3.1_left.stl"
    "fCalPhantom/fCal_3/fCal_3.1_spacer.stl"
  EDIT_LINK "${CATALOG_URL}/fCalPhantom/fCal_3"
  DESCRIPTION "Phantom for freehand spatial ultrasound calibration for deep structures (up to 30 cm)."
  )
MODEL_TABLE_END()  

MODEL_CATALOG_END()
