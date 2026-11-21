# Loop over files in src, 
#   Exclude arm folders
#   Find .h or .c files
#   Run linter
find ./Hype/ \
    -path './Hype/autogen' -prune -o \
    -path './Hype/conftool' -prune -o \
    -path './Hype/kernel/arm' -prune -o \
    -path './Hype/drivers/export' -prune -o \
    -regex "\./.*\.[c/h]\$" \
    -exec uncrustify -c ./Hype/linter/uncrustify-config.cfg --check {} +
