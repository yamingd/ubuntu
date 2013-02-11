CREATE DATABASE mogilefs;
GRANT ALL ON mogilefs.* TO 'mogile'@'%';
SET PASSWORD FOR 'mogile'@'%' = OLD_PASSWORD('mogilefs');
FLUSH PRIVILEGES;