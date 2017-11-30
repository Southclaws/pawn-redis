FROM maddinat0r/debian-samp

COPY . .

CMD [ "make", "all" ]
