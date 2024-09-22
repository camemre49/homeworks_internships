const crypto = require('crypto');

function generateSecretKey(length) {
  const byteLength = Math.ceil(length / 2);
  const randomBuffer = crypto.randomBytes(byteLength);
  return randomBuffer.toString('hex');
}

const secretKey = generateSecretKey(64);

module.exports = secretKey;
