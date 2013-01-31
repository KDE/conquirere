/*
 * Copyright 2012 Jörg Ehrichs <joerg.ehrichs@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mendeleyclient.h"

#include <QtOAuth/QtOAuth>
#include <QtCore/QUrl>

#include <QtCore/QDebug>

MendeleyClient::MendeleyClient()
    : QObject(0)
    , m_connected(false)
{
}

MendeleyClient::~MendeleyClient()
{

}

MendeleyClient* MendeleyClient::getClient()
{
    static MendeleyClient singeltonClient;
    return &singeltonClient;
}

void MendeleyClient::connectClient(const ProviderSyncDetails &psd)
{
    QByteArray token;
    QByteArray tokenSecret;

    m_qoauth = new QOAuth::Interface;
    // set the consumer key and secret
    m_qoauth->setConsumerKey( psd.userName.toLatin1() );
    m_qoauth->setConsumerSecret( psd.pwd.toLatin1() );
    // set a timeout for requests (in msecs)
    m_qoauth->setRequestTimeout( 10000 );

    // send a request for an unauthorized token
    QOAuth::ParamMap reply = m_qoauth->requestToken( "http://api.mendeley.com/oauth/request_token/", QOAuth::GET, QOAuth::PLAINTEXT );

    // if no error occurred, read the received token and token secret
    if ( m_qoauth->error() == QOAuth::NoError ) {
        m_requestToken = reply.value( QOAuth::tokenParameterName() );
        m_requestSecret = reply.value( QOAuth::tokenSecretParameterName());

        qDebug() << "version :: " << reply.value( QOAuth::supportedOAuthVersion() );
    }
    else {
        qDebug() << "ERROR!!!:" << m_qoauth->error();

        switch(m_qoauth->error()) {
        case QOAuth::BadRequest:
            qDebug() << "Represents HTTP status code 400 (Bad Request).";
            break;
        case QOAuth::Unauthorized:
            qDebug() << "Represents HTTP status code 401 (Unauthorized).";
            break;
        case QOAuth::Forbidden:
            qDebug() << "Represents HTTP status code 403 (Forbidden).";
            break;
        case QOAuth::Timeout:
            qDebug() << "Represents a request timeout error.";
            break;
        case QOAuth::ConsumerKeyEmpty:
            qDebug() << "Consumer key has not been provided.";
            break;
        case QOAuth::ConsumerSecretEmpty:
            qDebug() << "Consumer secret has not been provided.";
            break;
        case QOAuth::UnsupportedHttpMethod:
            qDebug() << "The HTTP method is not supported by the request. Note that QOAuth::Interface::requestToken() and QOAuth::Interface::accessToken() accept only HTTP GET and POST requests.";
            break;
        case QOAuth::RSAPrivateKeyEmpty:
            qDebug() << "RSA private key has not been provided.";
            break;
        case QOAuth::RSADecodingError:
            qDebug() << "There was a problem decoding the RSA private key (the key is invalid or the provided passphrase is incorrect)";
            break;
        case QOAuth::RSAKeyFileError:
            qDebug() << "The provided key file either doesn't exist or is unreadable.";
            break;
        case QOAuth::OtherError:
            qDebug() << "TOtherError";
            break;
        }

        return;
    }

    // ok we got a token, get the validation pin from the user
    QString url = QLatin1String("http://api.mendeley.com/oauth/authorize/?oauth_token=") + m_requestToken;// + QLatin1String("&oauth_callback=oob");

    emit verifyClient( url );
}

void MendeleyClient::clientVerified(const QString &pin)
{
    qDebug() << "verify pin ::" << pin << "token:" << m_requestToken << "secret:" << m_requestSecret;
    QByteArray token;
    QByteArray tokenSecret;

    QOAuth::ParamMap args;
    args.insert("oauth_verifier", pin.toLatin1());

    // send a request to exchange Request Token for an Access Token
    QOAuth::ParamMap reply = m_qoauth->accessToken( "http://api.mendeley.com/oauth/access_token", QOAuth::GET,
                                                    m_requestToken.toAscii(), m_requestSecret.toAscii(), QOAuth::HMAC_SHA1, args );

    // if no error occurred, read the Access Token (and other arguments, if applicable)
    if ( m_qoauth->error() == QOAuth::NoError ) {
      token = reply.value( QOAuth::tokenParameterName() );
      tokenSecret = reply.value( QOAuth::tokenSecretParameterName() );

      qDebug() << "access worked" << token << tokenSecret;
    }
    else {
        qDebug() << "ERROR!!!:" << m_qoauth->error();

        QMapIterator<QByteArray,QByteArray> i(reply);
         while (i.hasNext()) {
             i.next();
             qDebug() << i.key() << ": " << i.value();
         }

        qDebug() << reply.keys();
        switch(m_qoauth->error()) {
        case QOAuth::BadRequest:
            qDebug() << "Represents HTTP status code 400 (Bad Request).";
            break;
        case QOAuth::Unauthorized:
            qDebug() << "Represents HTTP status code 401 (Unauthorized).";
            break;
        case QOAuth::Forbidden:
            qDebug() << "Represents HTTP status code 403 (Forbidden).";
            break;
        case QOAuth::Timeout:
            qDebug() << "Represents a request timeout error.";
            break;
        case QOAuth::ConsumerKeyEmpty:
            qDebug() << "Consumer key has not been provided.";
            break;
        case QOAuth::ConsumerSecretEmpty:
            qDebug() << "Consumer secret has not been provided.";
            break;
        case QOAuth::UnsupportedHttpMethod:
            qDebug() << "The HTTP method is not supported by the request. Note that QOAuth::Interface::requestToken() and QOAuth::Interface::accessToken() accept only HTTP GET and POST requests.";
            break;
        case QOAuth::RSAPrivateKeyEmpty:
            qDebug() << "RSA private key has not been provided.";
            break;
        case QOAuth::RSADecodingError:
            qDebug() << "There was a problem decoding the RSA private key (the key is invalid or the provided passphrase is incorrect)";
            break;
        case QOAuth::RSAKeyFileError:
            qDebug() << "The provided key file either doesn't exist or is unreadable.";
            break;
        case QOAuth::OtherError:
            qDebug() << "TOtherError";
            break;
        }
    }
}

bool MendeleyClient::isConnected() const
{
    return !m_accessToken.isEmpty();
}
