#!/bin/bash

_rss()
{
    local cur prev subcommands
    COMPREPLY=()
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    subcommands="add fetch help init list remove"
    case "$prev" in
	"rss"|"help")
            COMPREPLY=( $(compgen -W "${subcommands}" -- $cur) )
            return 0
	    ;;
    esac
}

complete -F _rss rss

