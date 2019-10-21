#Ensure you source crab and setup voms certificate
for filename in Legacy*/crab_Legacy*/; do
  echo $filename
  crab status -d $filename
done
