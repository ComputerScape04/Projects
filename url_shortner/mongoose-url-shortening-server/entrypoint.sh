#!/bin/sh
echo "=== CONTAINER DIAGNOSTICS START ==="
echo "Current Directory: $(pwd)"
echo "Current User: $(whoami) (UID: $(id -u))"
echo "Listing files in /app:"
ls -la /app
echo "Checking binary dependencies (ldd):"
ldd /app/http_server
echo "Running the http_server..."
echo "==================================="
exec /app/http_server
