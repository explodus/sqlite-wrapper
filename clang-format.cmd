for /R %%f in (*.h *.hpp *.cpp *.cxx *.cc *.inl) do (
  clang-format.exe -i -style=file %%f
)
pause