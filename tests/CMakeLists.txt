# Copyright (c) 2017 ahmetbilgili@gmail.com
include(InstallFiles)

# Libraries to link the tests executables with
set(TEST_LIBRARIES Tuyau ${Boost_LIBRARIES})

# Create and install all the tests
include(CommonCTest)
install_files(share/Tuyau/tests FILES ${TEST_FILES} COMPONENT examples)
