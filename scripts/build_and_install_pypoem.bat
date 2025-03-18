pip install cibuildwheel
rm ./wheels -r -force
cibuildwheel --print-build-identifiers
cibuildwheel --output-dir wheels --only cp313-win_amd64 --debug-traceback --allow-empty

pip uninstall -y pypoem
cd .\wheels
