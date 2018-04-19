readandconvert <- function(fname,skipstr){
# Считать все данные без заголовка пропустить 20 строк с параметрами
dat <- read.table(fname, skip = skipstr, header =FALSE, sep =' ')

# найти в параметрах нужную строку и считать из нее нужные цифры
parstr <- grep("ABCc=", readLines(fname), value = TRUE)
# убрать лишнее
parstr <- gsub("ABCc=","",parstr)
# Заменить , на .
parstr <- gsub(",",".",parstr)

# Преобразовать строки в числа разбив строку на несколько с разделителем пробел
rspl <- as.numeric(unlist(strsplit(parstr," ")))
# Присвоить значения параметрам
A <- rspl[1]
B <- rspl[2]
CC <- rspl[3]

# Приведение к нулю по первому значению
dat$V2 <- dat$V2-dat$V2[1]



# Перевод температуры в градусы
dat$V1 <- dat$V1*(dat$V1*A+B)+CC

return(dat)
}
