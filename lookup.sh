#!/bin/sh
[ -z "$1" ] && exit 1
[ -f "docs.txt" ] || exit 1

stat=1
br=--------------------------------------------------------------------------------
matching=f
while read -r line; do
    if [ "$matching" = t ]; then
        case "$line" in
            *===*|*---*)
                matching=f
                ;;
            *)
                printf '  %s\n' "$line" ;;
        esac
    else
    case "$line" in
        *"$*"*:)
            matching=t
            stat=0
            read -r line2
            printf '%s\n\n %s\n %s\n' "$br" "$line" "$line2"
            ;;
    esac
    fi
done < docs.txt

[ "$stat" = 0 ] && printf '%s\n' "$br"

exit "$stat"
