import {Component, OnInit} from '@angular/core';
import {faRefresh, faTrash, faSortUp, faSortDown, faSort} from "@fortawesome/free-solid-svg-icons";
import {Message} from "../../../Message";
import {ApiService} from "../../services/api.service";
import {MenuService} from "../../services/menu.service";


@Component({
  selector: 'app-list-message',
  templateUrl: './list-message.component.html',
  styleUrls: ['./list-message.component.css']
})
export class ListMessageComponent implements OnInit {
  faRefresh = faRefresh;
  faTrash = faTrash;
  messages : Message[];
  inOrOut: string;
  currentPage: number =  0;
  totalPages: number;
  pageLimit: number;
  faSortUp =  faSortUp;
  faSortDown = faSortDown;
  sortType: string = "date";
  rew: number = 1;
  messageShow: boolean = false;
  messageContent: Message;
  messageId: string;

  onRefresh(){
    if(this.pageLimit){
      this.menuService.pageLimit = this.pageLimit;
      this.apiService.messagePageLimit = this.pageLimit;
    }
    else{
      this.menuService.pageLimit = 100
      this.apiService.messagePageLimit = 100
    }
    this.currentPage = 0;
    this.messages = [];
    this.ngOnInit();
  }


  ngOnInit() {
      this.apiService.getMessages(1, "date", -1).subscribe((messages) => {
        this.messages = messages["items"];
        if(this.inOrOut === "inbox"){
          this.menuService.setInboxItem(messages["totalItems"]);
        }
        else{
          this.menuService.setOutboxItem(messages["totalItems"]);
        }
        if(this.inOrOut === "inbox"){
          let n =this.menuService.inboxItem / this.menuService.pageLimit;
          if(n % 1 === 0){
            this.totalPages = n
          }
          else{
            this.totalPages = Math.floor(n) + 1;
          }
        }
        else{
          let n = this.menuService.outboxItem / this.menuService.pageLimit
          if(n % 1 === 0){
            this.totalPages = n;
          }
          else{
            this.totalPages = Math.floor(n) + 1;
          }
        }
    })
    this.inOrOut = this.apiService.getInOut()

  }

  constructor(private apiService: ApiService, private menuService: MenuService) {
    this.inOrOut = this.apiService.getInOut()
  }

  onDelete(id: string){
    if (!confirm('Are you sure?')){
      return;
    }
    this.apiService.deleteMessage(id).subscribe(((response) => {
      if(this.messageContent && id === this.messageContent._id){
        this.messageContent = null;
      }
      this.onRefresh();
      alert("Message deleted!")
      })
    );

  }

  sort(key: string, rev): void {
    this.currentPage = 0;
    this.sortType = key;
    this.rew = rev;
    this.apiService.getMessages(1, key, rev).subscribe((response) => {
      this.messages = response["items"]
    })
  }

  goPage(i: number){
    if(this.currentPage == i){
      return;
    }
    this.currentPage = i;
    this.apiService.getMessages(i + 1, this.sortType, this.rew).subscribe((messages) => {
      this.messages = messages["items"];
    })
  }

  pageNext(){
    if (this.currentPage === this.totalPages){
      return;
    }
    this.goPage(this.currentPage + 1)
  }
  pagePrev(){
    if (this.currentPage === 0){
      return;
    }
    this.goPage(this.currentPage - 1)
  }

  showMessage(message){
      this.messageShow = true;
      this.messageContent = message;
      this.messageId = message._id;
  }

  isSelected(id){
    return this.messageId === id;
  }

  getLocalDate(utcDate: string): string {
    const date = new Date(utcDate);
    date.setHours(date.getHours() + 3);
    return date.toISOString().substring(0, 10);
  }

  getLocalTime(utcDate: string): string {
    const date = new Date(utcDate);
    date.setHours(date.getHours() + 3);
    return date.toISOString().substring(11, 16);
  }


}
