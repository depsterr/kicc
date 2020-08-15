#!/bin/sh

# this script makes docs from the comments in the c header files

printf 'kicc internal docs\n'

state=code
br='  ------------------------------------------------------------------------------'
fbr='================================================================================'
for file in ./src/*.h; do
    printf '%s\n  %s\n' "$fbr" "${file##*/}"
    while read -r line; do
        case "$line" in
            *'/*'*'*/'*) ;;
            *'/*'*)
                state=comment 
                unset desc
                printf '%s\n\n' "$br"
                ;;
            *'*/'*)
                state=code
                read -r func
                funcname="${func%%(*}"
                funcname="${funcname##* }"
                funcname="${funcname%;}"
                printf "  %s:\n  %s\n\n$desc\n" "$funcname" "$func"
                ;;
            *)
                [ "$state" = comment ] && desc="$desc    $line\n"
        esac
    done < "$file"
done
printf '%s\n' "$br"
