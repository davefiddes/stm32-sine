#!/bin/bash
set -e

if [ -z "$1" ]; then
  echo "Usage: $0 <release-tag> [release-notes-file]"
  exit 1
fi

TAG="$1"
NOTES=""

if [ -n "$2" ]; then
  if [ -f "$2" ]; then
    NOTES="$(cat "$2")"
  else
    echo "Release notes file '$2' not found."
    exit 1
  fi
else
  echo "Enter release notes (end with Ctrl+D):"
  NOTES="$(cat)"
fi

# Tag the current commit with annotation
git tag -a "$TAG" -m "$NOTES"
git push origin "$TAG"

echo "Release tag '$TAG' pushed."

# Publish release to GitHub (requires gh CLI)
if command -v gh >/dev/null 2>&1; then
  gh release create "$TAG" --notes "$NOTES"
  echo "GitHub release created with notes."
else
  echo "gh CLI not found. Please create the GitHub release manually and include the release notes."
fi

echo "Now trigger the GitHub Actions 'Release Firmware' workflow with tag: $TAG"
