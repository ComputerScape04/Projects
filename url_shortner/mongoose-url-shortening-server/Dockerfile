FROM alpine:latest

# Install compilers and database dependencies (both dev headers and runtime libraries)
RUN apk add --no-cache \
    gcc \
    musl-dev \
    sqlite-dev \
    sqlite-libs

# Set application directory
WORKDIR /app

# Make the directory writable for SQLite file creation
RUN chmod 777 /app

# Copy all source files into the container
COPY . .

# Compile the URL shortener server directly in the runtime directory
RUN gcc -O2 *.c -lsqlite3 -o /app/http_server

# Make the entrypoint script executable
RUN chmod +x /app/entrypoint.sh

# Expose port 8000
EXPOSE 8000

# Start using the diagnostic entrypoint script
CMD ["/app/entrypoint.sh"]
