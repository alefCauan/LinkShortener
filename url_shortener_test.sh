#!/bin/bash

# URL de teste
LONG_URL="https://github.com/alefCauan/LinkShortener"

# 1. Encurtar URL
RESPONSE=$(curl -s -X POST http://localhost:8080/shorten \
    -H "Content-Type: application/json" \
    -d "{\"url\": \"$LONG_URL\"}")

echo "Response from shorten API:"
echo "$RESPONSE"

# 2. Extrair shortId usando jq (precisa do jq instalado)
SHORT_ID=$(echo "$RESPONSE" | jq -r '.shortId')

echo "Short ID: $SHORT_ID"

# 3. Testar redirect com -v para ver headers
echo "Testing redirect:"
curl -v http://localhost:8080/$SHORT_ID
