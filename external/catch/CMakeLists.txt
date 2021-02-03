# Testing library
FetchContent_Declare(
  catch
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.9.1
 )

  # Adds Catch2::Catch2
FetchContent_MakeAvailable(catch)
message(STATUS "Exporting Catch2::Catch2 target.")

