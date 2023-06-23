# install-module uefiv2
$BYTES = [byte] 255,255,255,255,41,41,41,41,255,255,255,255
Set-UEFIVariable -VariableName IOActive -Namespace "{8be4df61-1234-5432-asda-00ee0394855a}" -ByteArray $BYTES
